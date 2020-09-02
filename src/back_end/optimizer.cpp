/*
 * optimizer.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#include "optimizer.h"

#include <iterator>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <map>

char const *last_optimize_error_msg = nullptr;

int optimize = 0;

int get_optimize_level() {
	return optimize;
}
void set_optimize_level(int _optimize) {
	optimize = _optimize;
}

template<typename OPERATE>
bool calculation(std::vector<std::shared_ptr<Node>> const &parameters, std::shared_ptr<Node> const &parent, OPERATE operate,
		std::shared_ptr<LiteralNode> &literal) {
	std::shared_ptr<LiteralNode> operand0;
	std::shared_ptr<LiteralNode> operand1;
	if (!evaluate_literal_node(parameters[0], operand0))
		return false;
	if (!evaluate_literal_node(parameters[1], operand1))
		return false;
	literal = std::shared_ptr<LiteralNode>(new LiteralNode(parent, ND_LITERAL, calculation(operand0->getValue(), operand1->getValue(), operate)));
	return true;
}

bool evaluate_literal_node(std::shared_ptr<Node> const &node, std::shared_ptr<LiteralNode> &literal) {
	last_optimize_error_msg = nullptr;
	ND_TYPE type = node->getType();
	switch (type) {
	case ND_PARENTHESIS: {
		std::shared_ptr<ParenthesisNode> _node = std::dynamic_pointer_cast<ParenthesisNode>(node);
		if (!evaluate_literal_node(std::dynamic_pointer_cast<Node>(_node->getNode()), literal))
			return false;
	}
		break;
	case ND_LITERAL: {
		std::shared_ptr<LiteralNode> _node = std::dynamic_pointer_cast<LiteralNode>(node);
		literal = std::shared_ptr<LiteralNode>(new LiteralNode(_node));
	}
		break;
	case ND_OPERATION: {
		std::shared_ptr<OperationNode> _node = std::dynamic_pointer_cast<OperationNode>(node);
		OP_TYPE operator_type = _node->getOperatorType();
		std::vector<std::shared_ptr<Node>> const &parameters = _node->getParameters();
		std::shared_ptr<Node> const &parent = _node->getParent();
		switch (operator_type) {
		case OP_UNARY_PLUS:
			if (!evaluate_literal_node(parameters[0], literal))
				return false;
			literal->setParent(_node->getParent());
			break;
		case OP_UNARY_MINUS:
			if (!evaluate_literal_node(parameters[0], literal))
				return false;
			literal->setParent(_node->getParent());
			literal->setValue(-literal->getValue());
			break;
		case OP_LOGICAL_NOT:
			if (!evaluate_literal_node(parameters[0], literal))
				return false;
			literal->setParent(_node->getParent());
			literal->setValue(!literal->getValue());
			break;
		case OP_MUL:
			if (!calculation(parameters, parent, OPERATE_MUL(), literal))
				return false;
			break;
		case OP_DIV:
			if (!calculation(parameters, parent, OPERATE_DIV(), literal))
				return false;
			break;
		case OP_MOD:
			if (!calculation(parameters, parent, OPERATE_MOD(), literal))
				return false;
			break;
		case OP_ADD:
			if (!calculation(parameters, parent, OPERATE_ADD(), literal))
				return false;
			break;
		case OP_SUB:
			if (!calculation(parameters, parent, OPERATE_SUB(), literal))
				return false;
			break;
		case OP_GREATER:
			if (!calculation(parameters, parent, OPERATE_GREATER(), literal))
				return false;
			break;
		case OP_LESS:
			if (!calculation(parameters, parent, OPERATE_LESS(), literal))
				return false;
			break;
		case OP_GREATER_EQUAL:
			if (!calculation(parameters, parent, OPERATE_GREATER_EQUAL(), literal))
				return false;
			break;
		case OP_LESS_EQUAL:
			if (!calculation(parameters, parent, OPERATE_LESS_EQUAL(), literal))
				return false;
			break;
		case OP_EQUAL:
			if (!calculation(parameters, parent, OPERATE_EQUAL(), literal))
				return false;
			break;
		case OP_NOT_EQUAL:
			if (!calculation(parameters, parent, OPERATE_NOT_EQUAL(), literal))
				return false;
			break;
		case OP_LOGICAL_AND:
			if (!calculation(parameters, parent, OPERATE_LOGICAL_AND(), literal))
				return false;
			break;
		case OP_LOGICAL_OR:
			if (!calculation(parameters, parent, OPERATE_LOGICAL_OR(), literal))
				return false;
			break;
		case OP_ASSIGNMENT:
			last_optimize_error_msg = "Expression cannot contain assignment operation!";
			return false;
			break;
		default:
			last_optimize_error_msg = "Unknown operation!";
			return false;
			break;
		}
	}
		break;
	case ND_VAR: {
		std::shared_ptr<VarNode> _node = std::dynamic_pointer_cast<VarNode>(node);
		std::shared_ptr<VarTypeNode> const &target_var = _node->getTargetVar();
		std::shared_ptr<VarDefNode> const &var_def = std::dynamic_pointer_cast<VarDefNode>(target_var->getParent());
		if (!var_def->isIsConst()) {
			last_optimize_error_msg = "Target variable is not const in evaluate_literal_node!!!";
			return false;
		}
		std::vector<std::shared_ptr<VarTypeNode>> const &var_type = var_def->getVarType();
		int var_type_size = var_type.size();
		int index = -1;
		while (++index < var_type_size && target_var != var_type[index])
			;
		if (index >= var_type_size) {
			last_optimize_error_msg = "target_var is not in it parent's var_type vector!!!";
			return false;
		}
		std::shared_ptr<Node> const &_inti_value = var_def->getVarInitValue()[index];
		if (ND_LITERAL != _inti_value->getType()) {
			last_optimize_error_msg = "Const variable init value is not literal!!!";
			return false;
		}
		std::shared_ptr<LiteralNode> const &inti_value = std::dynamic_pointer_cast<LiteralNode>(_inti_value);
		literal = std::shared_ptr<LiteralNode>(new LiteralNode(inti_value));
	}
		break;
	case ND_TYPE_:
	case ND_VAR_TYPE:
	case ND_DEF:
	case ND_VAR_DEF:
	case ND_FUN_DEF:
	case ND_REF:
	case ND_FUN:
	case ND_STMT:
	case ND_IF_ELSE:
	case ND_WHILE:
	case ND_CONTINUE:
	case ND_BREAK:
	case ND_RETURN:
	case ND_BLOCK:
	case ND_ARRAY:
	case ND_ARRAY_ACCESS:
	case ND_UNKNOWN:
	default:
		last_optimize_error_msg = "Not supported node type in evaluate_literal_node!";
		// This is an error!!!
		return false;
	}
	return true;
}

template<class NODE_TYPE>
bool array_replace_node_function_call(std::vector<std::shared_ptr<NODE_TYPE>> &nodes, std::vector<std::string> &function_call_name,
		std::map<std::string, std::shared_ptr<Node>> &function_call) {
	for (typename std::vector<std::shared_ptr<NODE_TYPE>>::iterator it = nodes.begin(), end = nodes.end(); it != end; ++it) {
		if (!replace_node_function_call(*it, function_call_name, function_call))
			return false;
	}
	return true;
}

// todo:
bool is_volatile_function(char const *const function_name) {
	if (!strcmp("rand", function_name))
		return true;
	for (int i = 0; i < SY_SYSTEM_FUNCTION_LEN; ++i) {
		char const *const system_fun = SY_SYSTEM_FUNCTION[i];
		if (!strcmp(system_fun, function_name))
			return true;
	}
	return false;
}

// todo :
bool replace_node_function_call(std::shared_ptr<Node> &node, std::vector<std::string> &function_call_name,
		std::map<std::string, std::shared_ptr<Node>> &function_call) {
	ND_TYPE type = node->getType();
	switch (type) {
	case ND_FUN: {
		std::shared_ptr<FunNode> _node = std::dynamic_pointer_cast<FunNode>(node);
		if (is_volatile_function(_node->getName().c_str()))
			break;
		std::ostringstream oss;
		oss << "_____" << _node->getName();

		std::vector<std::shared_ptr<Node>> &parameters = _node->getParameters();
		for (std::vector<std::shared_ptr<Node>>::iterator it = parameters.begin(), end = parameters.end(); it != end; ++it) {
			ND_TYPE type = (*it)->getType();
			switch (type) {
			case ND_VAR:
				oss << "_" << std::dynamic_pointer_cast<VarNode>(*it)->getName();
				break;
			case ND_ARRAY_ACCESS: {
				std::shared_ptr<ArrayAccessNode> _node = std::dynamic_pointer_cast<ArrayAccessNode>(*it);
				if (!array_replace_node_function_call(_node->getIndexes(), function_call_name, function_call))
					return false;

				std::shared_ptr<Node> const &target = _node->getTarget();
				if (ND_VAR == target->getType())
					oss << "_" << std::dynamic_pointer_cast<VarNode>(target)->getName() << "[]";
			}
				break;
			case ND_LITERAL:
				oss << "_" << std::dynamic_pointer_cast<LiteralNode>(*it)->getValue();
				break;
			case ND_FUN:
				if (!replace_node_function_call(*it, function_call_name, function_call))
					return false;
				oss << "_" << std::dynamic_pointer_cast<VarNode>(*it)->getName();
				break;
			case ND_PARENTHESIS: {
				std::shared_ptr<ParenthesisNode> _node = std::dynamic_pointer_cast<ParenthesisNode>(*it);
				if (!replace_node_function_call(_node->getNode(), function_call_name, function_call))
					return false;
			}
				break;
			case ND_OPERATION: {
				std::shared_ptr<OperationNode> _node = std::dynamic_pointer_cast<OperationNode>(*it);
				if (!array_replace_node_function_call(_node->getParameters(), function_call_name, function_call))
					return false;
				oss << "_operation_type" << _node->getOperatorType();
			}
				break;
			default:
				oss << "_type" << type;
				break;
			}
		}
		std::string name = oss.str();
		if (function_call.find(name) == function_call.end()) {
			function_call_name.push_back(name);
			function_call.insert(std::pair<std::string, std::shared_ptr<Node> >(name, _node));
		}
		std::shared_ptr<VarNode> var_node(new VarNode(node, ND_VAR, name.c_str()));
		node = std::dynamic_pointer_cast<Node>(var_node);
	}
		break;
	case ND_STMT:
	case ND_PARENTHESIS: {
		std::shared_ptr<ParenthesisNode> _node = std::dynamic_pointer_cast<ParenthesisNode>(node);
		if (!replace_node_function_call(_node->getNode(), function_call_name, function_call))
			return false;
	}
		break;
	case ND_ARRAY_ACCESS: {
		std::shared_ptr<ArrayAccessNode> _node = std::dynamic_pointer_cast<ArrayAccessNode>(node);
		if (!array_replace_node_function_call(_node->getIndexes(), function_call_name, function_call))
			return false;
	}
		break;
	case ND_OPERATION: {
		std::shared_ptr<OperationNode> _node = std::dynamic_pointer_cast<OperationNode>(node);
		if (!array_replace_node_function_call(_node->getParameters(), function_call_name, function_call))
			return false;
	}
		break;
	case ND_VAR:
	case ND_LITERAL:
	case ND_ARRAY:
		break;
	case ND_TYPE_:
	case ND_VAR_TYPE:
	case ND_VAR_DEF:
	case ND_FUN_DEF:
	case ND_IF_ELSE:
	case ND_WHILE:
	case ND_CONTINUE:
	case ND_BREAK:
	case ND_RETURN:
	case ND_BLOCK:
	case ND_DEF:
	case ND_REF:
	case ND_UNKNOWN:
	default:
		last_optimize_error_msg = "Not supported node type while replace_node_function_call!!";
		return false;
	}
	return true;
}

bool merge_block_function_call(std::shared_ptr<BlockNode> &block) {
	std::vector<std::shared_ptr<Node>> &children = block->getChildren();
	std::map<std::string, std::shared_ptr<Node>> function_call;
	std::vector<std::string> function_call_name;
	int index = 0;
	for (int index = 0, end = children.size(); index < end; ++index) {
		std::shared_ptr<Node> &node = children[index];
		if (ND_STMT == node->getType()) {
			std::shared_ptr<StmtNode> stmt = std::dynamic_pointer_cast<StmtNode>(node);
			std::shared_ptr<Node> &expression = stmt->getStmt();
			if (!replace_node_function_call(expression, function_call_name, function_call))
				return false;

			for (std::vector<std::string>::iterator _it = function_call_name.begin(), _end = function_call_name.end(); _it != _end; ++_it) {
				std::string &first = *_it;
				std::shared_ptr<Node> &second = function_call[*_it];
				if (nullptr == second)
					continue;
				std::shared_ptr<VarTypeNode> var_type_node(new VarTypeNode(nullptr, ND_VAR_TYPE, first.c_str()));
				std::shared_ptr<TypeNode> _type(new TypeNode(nullptr, ND_TYPE_, VA_INT));
				std::shared_ptr<VarDefNode> var_def_node(new VarDefNode(std::dynamic_pointer_cast<Node>(block), ND_VAR_DEF, "", _type, false));
				var_def_node->add_var(var_type_node, second);
				second->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
				var_type_node->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
				_type->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
//				std::shared_ptr<StmtNode> stmt_node(new StmtNode(std::dynamic_pointer_cast<Node>(block), ND_STMT, var_def_node));
//				var_def_node->setParent(std::dynamic_pointer_cast<Node>(stmt_node));
				children.insert(children.begin() + index, std::dynamic_pointer_cast<Node>(var_def_node));
				++index;
				++end;
				second = nullptr;
			}
//			for (std::map<std::string, std::shared_ptr<Node>>::iterator _it = function_call.begin(), _end = function_call.end(); _it != _end; ++_it) {
//				if (nullptr == _it->second)
//					continue;
//				std::shared_ptr<VarTypeNode> var_type_node(new VarTypeNode(nullptr, ND_VAR_TYPE, _it->first.c_str()));
//				std::shared_ptr<TypeNode> _type(new TypeNode(nullptr, ND_TYPE_, VA_INT));
//				std::shared_ptr<VarDefNode> var_def_node(new VarDefNode(std::dynamic_pointer_cast<Node>(block), ND_VAR_DEF, "", _type, false));
//				var_def_node->add_var(var_type_node, _it->second);
//				_it->second->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
//				var_type_node->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
//				_type->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
////				std::shared_ptr<StmtNode> stmt_node(new StmtNode(std::dynamic_pointer_cast<Node>(block), ND_STMT, var_def_node));
////				var_def_node->setParent(std::dynamic_pointer_cast<Node>(stmt_node));
//				children.insert(children.begin() + index, std::dynamic_pointer_cast<Node>(var_def_node));
//				++index;
//				++end;
//				_it->second = nullptr;
//			}
		}
	}
	return true;
}
