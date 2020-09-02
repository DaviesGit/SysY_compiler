/*
 * semantic_analyzer.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#include "semantic_analyzer.h"
#include "../back_end/optimizer.h"
#include "../common/global.h"

char const *last_semantic_analyze_error_msg = nullptr;

inline bool semantic_analyze(std::shared_ptr<Node> const &node, int block_id);
bool semantic_analyze(std::shared_ptr<Node> &node, int block_id);

bool find_previous_xxx_node(std::shared_ptr<Node> const &node, ND_TYPE type, std::shared_ptr<Node> &_node) {
	std::shared_ptr<Node> parent = node;
	while (parent = parent->getParent()) { // @suppress("Assignment in condition")
		if (parent->getType() != type)
			continue;
		_node = parent;
		return true;
	}
	return false;
}

bool find_previous_while_node(std::shared_ptr<Node> const &node, std::shared_ptr<WhileNode> &while_node) {
	std::shared_ptr<Node> _node;
	if (!find_previous_xxx_node(node, ND_WHILE, _node))
		return false;
	while_node = std::dynamic_pointer_cast<WhileNode>(_node);
	return true;
}

bool find_previous_fun_def_node(std::shared_ptr<Node> const &node, std::shared_ptr<FunDefNode> &fun_def_node) {
	std::shared_ptr<Node> _node;
	if (!find_previous_xxx_node(node, ND_FUN_DEF, _node))
		return false;
	fun_def_node = std::dynamic_pointer_cast<FunDefNode>(_node);
	return true;
}

bool find_target_var(std::shared_ptr<VarNode> const &var_node, std::shared_ptr<VarTypeNode> &target_var) {
	std::shared_ptr<Node> parent = std::dynamic_pointer_cast<Node>(var_node);
	std::string const &name = var_node->getName();
	while (parent = parent->getParent()) { // @suppress("Assignment in condition")
		ND_TYPE type = parent->getType();
		if (ND_BLOCK == type) {
			std::shared_ptr<BlockNode> _parent = std::dynamic_pointer_cast<BlockNode>(parent);
			std::vector<std::shared_ptr<Node> > const &children = _parent->getChildren();
			for (std::vector<std::shared_ptr<Node>>::const_iterator it = children.begin(), end = children.end(); it != end; ++it) {
				if (ND_VAR_DEF != (*it)->getType())
					continue;
				std::vector<std::shared_ptr<VarTypeNode>> const &var_type = std::dynamic_pointer_cast<VarDefNode>(*it)->getVarType();
				for (std::vector<std::shared_ptr<VarTypeNode>>::const_iterator it = var_type.begin(), end = var_type.end(); it != end; ++it) {
					if (name == (*it)->getName()) {
						target_var = *it;
						return true;
					}
				}
			}
		} else if (ND_FUN_DEF == type) {
			std::vector<std::shared_ptr<VarDefNode>> const &var_def = std::dynamic_pointer_cast<FunDefNode>(parent)->getParameters();
			for (std::vector<std::shared_ptr<VarDefNode>>::const_iterator it = var_def.begin(), end = var_def.end(); it != end; ++it) {
				std::vector<std::shared_ptr<VarTypeNode>> const &var_type = (*it)->getVarType();
				for (std::vector<std::shared_ptr<VarTypeNode>>::const_iterator it = var_type.begin(), end = var_type.end(); it != end; ++it) {
					if (name == (*it)->getName()) {
						target_var = *it;
						return true;
					}
				}
			}
		}

	}
	return false;
}

bool find_target_fun(std::shared_ptr<FunNode> const &fun_node, bool &is_system_fun, std::shared_ptr<FunDefNode> &target_fun) {
	is_system_fun = false;
	std::string const &name = fun_node->getName();
	for (int i = 0; i < SY_SYSTEM_FUNCTION_LEN; ++i) {
		char const *const system_fun = SY_SYSTEM_FUNCTION[i];
		if (name == system_fun) {
			is_system_fun = true;
			return true;
		}
	}
	std::shared_ptr<Node> parent = std::dynamic_pointer_cast<Node>(fun_node);
	while (parent = parent->getParent()) { // @suppress("Assignment in condition")
		ND_TYPE type = parent->getType();
		if (ND_BLOCK != type)
			continue;
		std::shared_ptr<BlockNode> _parent = std::dynamic_pointer_cast<BlockNode>(parent);
		std::vector<std::shared_ptr<Node> > const &children = _parent->getChildren();
		for (std::vector<std::shared_ptr<Node>>::const_iterator it = children.begin(), end = children.end(); it != end; ++it) {
			if (ND_FUN_DEF != (*it)->getType())
				continue;
			std::shared_ptr<FunDefNode> const &current_fun = std::dynamic_pointer_cast<FunDefNode>(*it);
			if (current_fun->getName() == name) {
				target_fun = current_fun;
				return true;
			}
		}
	}
	return false;
}

//bool unary_array_operation(OP_TYPE operator_type,) {
//
//	return true;
//}
//
//void variable2array(std::shared_ptr<VarTypeNode> const &var_type, std::shared_ptr<ArrayNode> &var_type) {
//
//}
//
//bool refactor_array_operation(std::shared_ptr<Node> &node) {
//	std::shared_ptr<OperationNode> _node = std::dynamic_pointer_cast<OperationNode>(node);
//	std::vector<std::shared_ptr<Node> > &parameters = _node->getParameters();
//	OP_TYPE operator_type = _node->getOperatorType();
//
//	switch (operator_type) {
//	case OP_UNARY_PLUS:
//	case OP_UNARY_MINUS:
//	case OP_LOGICAL_NOT:
//		std::shared_ptr<Node> &parameter = parameters[0];
//		ND_TYPE type = parameter->getType();
//		switch (type) {
//		case ND_VAR: {
//			std::shared_ptr<VarNode> const &_node = std::dynamic_pointer_cast<VarNode>(node);
//			const std::shared_ptr<VarTypeNode> &var_type = _node->getTargetVar();
//			std::vector<std::shared_ptr<Node>> &dimensions = var_type->getDimensions();
//			if (dimensions.empty())
//				return true;
//
//		}
//			break;
//		case ND_FUN: {
//			std::shared_ptr<FunNode> const &_node = std::dynamic_pointer_cast<FunNode>(node);
//			oss << "type:" << "\"ND_FUN\"" << ",";
//			oss << "name:" << "\"" << _node->getName() << "\"" << ",";
//			std::vector<std::shared_ptr<Node> >const &parameters = _node->getParameters();
//			oss << "parameters:" << "[";
//			for (std::vector<std::shared_ptr<Node> >::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it) {
//				dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
//				oss << ",";
//			}
//			oss << "]" << ",";
//
//		}
//			break;
//		case ND_PARENTHESIS: {
//			std::shared_ptr<ParenthesisNode> const &_node = std::dynamic_pointer_cast<ParenthesisNode>(node);
//			oss << "type:" << "\"ND_PARENTHESIS\"" << ",";
//			oss << "node:";
//			dump_node2json(std::dynamic_pointer_cast<Node>(_node->getNode()), oss);
//			oss << ",";
//
//		}
//			break;
//		case ND_LITERAL: {
//			std::shared_ptr<LiteralNode> const &_node = std::dynamic_pointer_cast<LiteralNode>(node);
//			oss << "type:" << "\"ND_LITERAL\"" << ",";
//			oss << "value:" << _node->getValue() << ",";
//
//		}
//			break;
//		case ND_ARRAY: {
//			std::shared_ptr<ArrayNode> const &_node = std::dynamic_pointer_cast<ArrayNode>(node);
//			oss << "type:" << "\"ND_ARRAY\"" << ",";
//			std::vector<std::shared_ptr<Node> >const &array = _node->getArray();
//			oss << "array:" << "[";
//
//			for (std::vector<std::shared_ptr<Node> >::const_iterator it = array.begin(), end = array.end(); it != end; ++it) {
//				dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
//				oss << ",";
//			}
//			oss << "]" << ",";
//
//		}
//			break;
//		case ND_ARRAY_ACCESS: {
//			std::shared_ptr<ArrayAccessNode> const &_node = std::dynamic_pointer_cast<ArrayAccessNode>(node);
//			oss << "type:" << "\"ND_ARRAY_ACCESS\"" << ",";
//			std::vector<std::shared_ptr<Node> >const &indexes = _node->getIndexes();
//			oss << "indexes:";
//			build_array(indexes, oss);
//			oss << ",";
//			oss << "target:";
//			dump_node2json(std::dynamic_pointer_cast<Node>(_node->getTarget()), oss);
//			oss << ",";
//		}
//			break;
//		case ND_TYPE_:
//		case ND_VAR_TYPE:
//		case ND_DEF:
//		case ND_VAR_DEF:
//		case ND_FUN_DEF:
//		case ND_REF:
//		case ND_STMT:
//		case ND_OPERATION:
//		case ND_IF_ELSE:
//		case ND_WHILE:
//		case ND_CONTINUE:
//		case ND_BREAK:
//		case ND_RETURN:
//		case ND_BLOCK:
//		case ND_UNKNOWN:
//		default:
//			last_semantic_analyze_error_msg = "Not supported array type!";
//			return false;
//		}
////		case OP_MUL:
////			oss << "/* *  */";
////			break;
////		case OP_DIV:
////			oss << "/* /  */";
////			break;
////		case OP_MOD:
////			oss << "/* %  */";
////			break;
////		case OP_ADD:
////			oss << "/* +  */";
////			break;
////		case OP_SUB:
////			oss << "/* -  */";
////			break;
////		case OP_GREATER:
////			oss << "/* >  */";
////			break;
////		case OP_LESS:
////			oss << "/* <  */";
////			break;
////		case OP_GREATER_EQUAL:
////			oss << "/* >= */";
////			break;
////		case OP_LESS_EQUAL:
////			oss << "/* <= */";
////			break;
////		case OP_EQUAL:
////			oss << "/* == */";
////			break;
////		case OP_NOT_EQUAL:
////			oss << "/* != */";
////			break;
////		case OP_LOGICAL_AND:
////			oss << "/* && */";
////			break;
////		case OP_LOGICAL_OR:
////			oss << "/* || */";
////			break;
////		case OP_ASSIGNMENT:
////			oss << "/* =  */";
////			break;
////		default:
////			oss << "This is an error!";
////			break;
//	}
//
//	return true;
//}

template<class NODE_TYPE>
bool array_semantic_analyze(std::vector<std::shared_ptr<NODE_TYPE>> &nodes, int block_id, bool allow_nullptr = false) {
	for (typename std::vector<std::shared_ptr<NODE_TYPE>>::iterator it = nodes.begin(), end = nodes.end(); it != end; ++it) {
		if (allow_nullptr && !*it)
			continue;
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(*it), block_id))
			return false;
	}
	return true;
}

inline bool semantic_analyze(std::shared_ptr<Node> const &node, int block_id) {
	std::shared_ptr<Node> __node(node);
	return semantic_analyze(__node, block_id);
}

bool semantic_analyze(std::shared_ptr<Node> &node, int block_id) {
	last_semantic_analyze_error_msg = nullptr;
	ND_TYPE type = node->getType();
	switch (type) {
	case ND_TYPE_: {
		std::shared_ptr<TypeNode> _node = std::dynamic_pointer_cast<TypeNode>(node);
		std::vector<std::shared_ptr<Node>> &dimensions = _node->getDimensions();
		for (std::vector<std::shared_ptr<Node>>::iterator it = dimensions.begin(), end = dimensions.end(); it != end; ++it) {
			std::shared_ptr<LiteralNode> literal;
			if (!evaluate_literal_node(*it, literal)) {
				last_semantic_analyze_error_msg = last_optimize_error_msg;
				return false;
			}
			literal->setParent(node);
			*it = std::dynamic_pointer_cast<Node>(literal);
		}
	}
		break;
	case ND_VAR_TYPE: {
		std::shared_ptr<VarTypeNode> _node = std::dynamic_pointer_cast<VarTypeNode>(node);
		std::vector<std::shared_ptr<Node>> &dimensions = _node->getDimensions();
		for (std::vector<std::shared_ptr<Node>>::iterator it = dimensions.begin(), end = dimensions.end(); it != end; ++it) {
			std::shared_ptr<LiteralNode> literal;
			if (!semantic_analyze(std::dynamic_pointer_cast<Node>(*it), block_id))
				return false;
			if (!evaluate_literal_node(*it, literal)) {
				last_semantic_analyze_error_msg = last_optimize_error_msg;
				return false;
			}
			literal->setParent(node);
			*it = std::dynamic_pointer_cast<Node>(literal);
		}
	}
		break;
	case ND_VAR_DEF: {
		std::shared_ptr<VarDefNode> _node = std::dynamic_pointer_cast<VarDefNode>(node);
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getVariableType()), block_id))
			return false;
		std::vector<std::shared_ptr<VarTypeNode> > &var_type = _node->getVarType();
		if (!array_semantic_analyze(var_type, block_id))
			return false;
		std::vector<std::shared_ptr<Node> > &var_inti_value = _node->getVarInitValue();
		if (!array_semantic_analyze(var_inti_value, block_id, true))
			return false;
		if (_node->isIsConst()) {
			for (int i = 0, end = var_type.size(); i < end; ++i) {
				std::shared_ptr<Node> const &init_value = var_inti_value[i];
				if (init_value && !var_type[i]->getDimensions().size()) {
					std::shared_ptr<LiteralNode> literal;
					if (!evaluate_literal_node(init_value, literal)) {
						last_semantic_analyze_error_msg = last_optimize_error_msg;
						return false;
					}
					literal->setParent(node);
				}
			}
		}
		_node->setBlockId(block_id);
	}
		break;
	case ND_FUN_DEF: {
		std::shared_ptr<FunDefNode> _node = std::dynamic_pointer_cast<FunDefNode>(node);
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getReturnType()), block_id))
			return false;
		if (!array_semantic_analyze(_node->getParameters(), block_id))
			return false;
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getBody()), block_id))
			return false;
		_node->setBlockId(block_id);
	}
		break;
	case ND_VAR: {
		std::shared_ptr<VarNode> _node = std::dynamic_pointer_cast<VarNode>(node);
		std::shared_ptr<VarTypeNode> target_var;
		if (!find_target_var(_node, target_var)) {
			last_semantic_analyze_error_msg = "Cannot find target variable definition!";
			return false;
		}
		_node->setTargetVar(target_var);

	}
		break;
	case ND_FUN: {
		std::shared_ptr<FunNode> _node = std::dynamic_pointer_cast<FunNode>(node);
		if (!array_semantic_analyze(_node->getParameters(), block_id))
			return false;
		bool is_system_fun = false;
		std::shared_ptr<FunDefNode> target_fun;
		if (!find_target_fun(_node, is_system_fun, target_fun)) {
			last_semantic_analyze_error_msg = "Cannot find target function definition!";
			return false;
		}
		_node->setIsSystemFun(is_system_fun);
		_node->setTargetFun(target_fun);

	}
		break;
	case ND_STMT: {
		std::shared_ptr<StmtNode> _node = std::dynamic_pointer_cast<StmtNode>(node);
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getStmt()), block_id))
			return false;
	}
		break;
	case ND_PARENTHESIS: {
		std::shared_ptr<ParenthesisNode> _node = std::dynamic_pointer_cast<ParenthesisNode>(node);
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getNode()), block_id))
			return false;

	}
		break;
	case ND_LITERAL: {
	}
		break;
	case ND_ARRAY: {
		std::shared_ptr<ArrayNode> _node = std::dynamic_pointer_cast<ArrayNode>(node);
		if (!array_semantic_analyze(_node->getArray(), block_id))
			return false;

	}
		break;
	case ND_ARRAY_ACCESS: {
		std::shared_ptr<ArrayAccessNode> _node = std::dynamic_pointer_cast<ArrayAccessNode>(node);
		if (!array_semantic_analyze(_node->getIndexes(), block_id))
			return false;
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getTarget()), block_id))
			return false;
	}
		break;
	case ND_OPERATION: {
		std::shared_ptr<OperationNode> _node = std::dynamic_pointer_cast<OperationNode>(node);
		std::vector<std::shared_ptr<Node> > &parameters = _node->getParameters();
		for (std::vector<std::shared_ptr<Node>>::iterator it = parameters.begin(), end = parameters.end(); it != end; ++it) {
			if (!semantic_analyze(*it, block_id))
				return false;
		}

	}
		break;
	case ND_IF_ELSE: {
		std::shared_ptr<IfElseNode> _node = std::dynamic_pointer_cast<IfElseNode>(node);
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getCondition()), block_id))
			return false;
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getIfBody()), block_id))
			return false;
		const std::shared_ptr<BlockNode> &else_body = _node->getElseBody();
		if (else_body)
			if (!semantic_analyze(std::dynamic_pointer_cast<Node>(else_body), block_id))
				return false;
	}
		break;
	case ND_WHILE: {
		std::shared_ptr<WhileNode> _node = std::dynamic_pointer_cast<WhileNode>(node);
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getCondition()), block_id))
			return false;
		if (!semantic_analyze(std::dynamic_pointer_cast<Node>(_node->getBody()), block_id))
			return false;
	}
		break;
	case ND_CONTINUE: {
		std::shared_ptr<ContinueNode> _node = std::dynamic_pointer_cast<ContinueNode>(node);
		std::shared_ptr<WhileNode> while_node;
		if (!find_previous_while_node(node, while_node)) {
			last_semantic_analyze_error_msg = "continue statement not in while loop!";
			return false;
		}
		_node->setTarget(while_node);

	}
		break;
	case ND_BREAK: {
		std::shared_ptr<BreakNode> _node = std::dynamic_pointer_cast<BreakNode>(node);
		std::shared_ptr<WhileNode> while_node;
		if (!find_previous_while_node(node, while_node)) {
			last_semantic_analyze_error_msg = "break statement not in while loop!";
			return false;
		}
		_node->setTarget(while_node);
	}
		break;
	case ND_RETURN: {
		std::shared_ptr<ReturnNode> _node = std::dynamic_pointer_cast<ReturnNode>(node);
		std::shared_ptr<FunDefNode> fun_def_node;
		if (!find_previous_fun_def_node(node, fun_def_node)) {
			last_semantic_analyze_error_msg = "return statement not in function definition node!";
			return false;
		}
		_node->setTarget(fun_def_node);
		const std::shared_ptr<Node> &return_value = _node->getReturnValue();
		if (return_value)
			if (!semantic_analyze(std::dynamic_pointer_cast<Node>(return_value), block_id))
				return false;
	}
		break;
	case ND_BLOCK: {
		std::shared_ptr<BlockNode> _node = std::dynamic_pointer_cast<BlockNode>(node);
		int block_id = next_global_id();
		_node->setId(block_id);
		if (2 == get_optimize_level())
			merge_block_function_call(_node);

		if (!array_semantic_analyze(_node->getChildren(), block_id))
			return false;
	}
		break;
	case ND_DEF:
	case ND_REF:
	case ND_UNKNOWN:
	default:
		last_semantic_analyze_error_msg = "Not supported node type!!";
		return false;
	}
	return true;
}

bool semantic_analyze(std::shared_ptr<BlockNode> &root) {
	last_semantic_analyze_error_msg = nullptr;
	if (!semantic_analyze(std::dynamic_pointer_cast<Node>(root), -1))
		return false;

	std::shared_ptr<FunDefNode> main;
	std::dynamic_pointer_cast<FunDefNode>(main);
	std::shared_ptr<BlockNode> block_node(new BlockNode(nullptr, ND_BLOCK));

	std::vector<std::shared_ptr<Node> > const &children = root->getChildren();
	for (std::vector<std::shared_ptr<Node>>::const_iterator it = children.begin(), end = children.end(); it != end; ++it) {
		switch ((*it)->getType()) {
		case ND_VAR_DEF: {
			std::shared_ptr<VarDefNode> var_def = std::dynamic_pointer_cast<VarDefNode>(*it);
			var_def->setIsGlobal(true);
			std::vector<std::shared_ptr<VarTypeNode>> const &var_type = var_def->getVarType();
			std::vector<std::shared_ptr<Node>> const &var_init_value = var_def->getVarInitValue();
			int index = 0;
			for (std::vector<std::shared_ptr<VarTypeNode>>::const_iterator it = var_type.begin(), end = var_type.end(); it != end; ++it) {
				std::vector<std::shared_ptr<Node>> &dimensions = (*it)->getDimensions();
				if (dimensions.size() && !var_init_value[index]) {
					int total_size = 1;
					for (std::vector<std::shared_ptr<Node>>::iterator it = dimensions.begin(), end = dimensions.end(); it != end; ++it) {
						if (ND_LITERAL == (*it)->getType())
							total_size *= std::dynamic_pointer_cast<LiteralNode>(*it)->getValue();
						else {
							last_semantic_analyze_error_msg = "Array dimension must be literal!";
							return false;
						}
					}
					std::shared_ptr<LiteralNode> literal(new LiteralNode(nullptr, ND_LITERAL, total_size * 4));
					std::shared_ptr<FunNode> fun_node(new FunNode(nullptr, ND_FUN, "malloc", literal));
					std::shared_ptr<VarNode> var_node(new VarNode(nullptr, ND_VAR, (*it)->getName().c_str()));
					std::shared_ptr<OperationNode> expr_node(new OperationNode(nullptr, ND_OPERATION, OP_ASSIGNMENT, var_node, fun_node));
					std::shared_ptr<StmtNode> stmt_node(new StmtNode(std::dynamic_pointer_cast<Node>(block_node), ND_STMT, expr_node));
					block_node->add_child(std::dynamic_pointer_cast<Node>(stmt_node));
					expr_node->setParent(std::dynamic_pointer_cast<Node>(stmt_node));
					var_node->setParent(std::dynamic_pointer_cast<Node>(expr_node));
					fun_node->setParent(std::dynamic_pointer_cast<Node>(expr_node));
					literal->setParent(std::dynamic_pointer_cast<Node>(fun_node));
					fun_node->setIsSystemFun(true);
					var_node->setTargetVar(*it);
				}
				++index;
			}
		}
			break;
		case ND_FUN_DEF: {
			std::shared_ptr<FunDefNode> fun = std::dynamic_pointer_cast<FunDefNode>(*it);
			if ("main" == fun->getName())
				main = std::dynamic_pointer_cast<FunDefNode>(*it);
		}
			break;
		}
	}
	std::shared_ptr<BlockNode> old_body = main->getBody();
	std::vector<std::shared_ptr<Node> > const &_children = old_body->getChildren();
	main->setBody(block_node);
	for (std::vector<std::shared_ptr<Node>>::const_iterator it = _children.begin(), end = _children.end(); it != end; ++it) {
		block_node->add_child(*it);
		(*it)->setParent(std::dynamic_pointer_cast<Node>(block_node));
	}
	block_node->setParent(std::dynamic_pointer_cast<Node>(main));
	return true;
}

