/*
 * parser.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#include <string.h>
#include <stack>
#include <sstream>

#include "parser.h"
#include "tokenize.h"
#include "../common/global.h"

char const *last_parse_error_msg = nullptr;

inline OP_TYPE operator_type(char const *const op, bool is_unary) {
	if (is_unary) {
		if (!strcmp("+", op))
			return OP_UNARY_PLUS;
		if (!strcmp("-", op))
			return OP_UNARY_MINUS;
		if (!strcmp("!", op))
			return OP_LOGICAL_NOT;
	} else {
		if (!strcmp("*", op))
			return OP_MUL;
		if (!strcmp("/", op))
			return OP_DIV;
		if (!strcmp("%", op))
			return OP_MOD;
		if (!strcmp("+", op))
			return OP_ADD;
		if (!strcmp("-", op))
			return OP_SUB;
		if (!strcmp(">", op))
			return OP_GREATER;
		if (!strcmp("<", op))
			return OP_LESS;
		if (!strcmp(">=", op))
			return OP_GREATER_EQUAL;
		if (!strcmp("<=", op))
			return OP_LESS_EQUAL;
		if (!strcmp("==", op))
			return OP_EQUAL;
		if (!strcmp("!=", op))
			return OP_NOT_EQUAL;
		if (!strcmp("&&", op))
			return OP_LOGICAL_AND;
		if (!strcmp("||", op))
			return OP_LOGICAL_OR;
		if (!strcmp("=", op))
			return OP_ASSIGNMENT;
	}
	return OP_UNKNOWN;
}

inline OPERATOR_INFO const& operator_info(OP_TYPE op) {
	return OPERATOR_INFO_MAP[op];
}

inline int operator_precedence_compare(OP_TYPE op0, OP_TYPE op1) {
	return operator_info(op0).precedence - operator_info(op1).precedence;
}

inline SY_ASSOCIATIVITY operator_associativity(OP_TYPE op) {
	return operator_info(op).associativity;
}

inline int operator_parameter_number(OP_TYPE op) {
	return operator_info(op).parameter_number;
}

inline bool expect(std::vector<std::shared_ptr<Token>>::const_iterator &current, std::vector<std::shared_ptr<Token>>::const_iterator &end,
		char const *const str) {
	return current != end && (*current)->getName() == str && (++current, 1);
}

bool gen_top_operator_operation_node(std::stack<OP_TYPE> &operator_stack, std::stack<std::shared_ptr<Node>> &node_stack) {
	if (operator_stack.empty())
		return false;
	OP_TYPE top_operator = operator_stack.top();
	operator_stack.pop();
	std::shared_ptr<OperationNode> expr_node(new OperationNode());
	expr_node->setType(ND_OPERATION);
	for (int i = 0, end = operator_parameter_number(top_operator); i < end; ++i) {
		if (node_stack.empty())
			return false;
		std::shared_ptr<Node> &node = node_stack.top();
		//This is reversed!!
		expr_node->add_parameter(node);
		node->setParent(std::dynamic_pointer_cast<Node>(expr_node));
		node_stack.pop();
	}
	expr_node->reverse_parameters();
	expr_node->setOperatorType(top_operator);
	std::shared_ptr<Node> node = std::dynamic_pointer_cast<Node>(expr_node);
	node_stack.push(node);
	return true;
}

//Although the return type is Node, but it actually do gen_expr_node work!
/* return Any node of  VarNode|FunNode|LiteralNode|ArrayNode|OperationNode|ParenthesisNode|ArrayAccessNode */
std::shared_ptr<Node> gen_expr_node(std::vector<std::shared_ptr<Token>>::const_iterator &current, std::vector<std::shared_ptr<Token>>::const_iterator &end) {

	bool previous_is_operator = true;

	std::stack<OP_TYPE> operator_stack;
	std::stack<std::shared_ptr<Node>> node_stack;

	for (TK_TYPE type = (*current)->getType();; type = (*current)->getType()) {
		switch (type) {
		// Operation node
		case TK_OPERATOR: {
			// Unary
			if (previous_is_operator) {
				OP_TYPE type = operator_type((*current)->getName().c_str(), true);
				if (OP_UNKNOWN == type)
					return nullptr;
				operator_stack.push(type);
			} else {
				OP_TYPE type = operator_type((*current)->getName().c_str(), false);
				if (OP_UNKNOWN == type)
					return nullptr;
				if (AS_L2R == operator_associativity(type)) {
					while (!operator_stack.empty() && 0 <= operator_precedence_compare(operator_stack.top(), type))
						gen_top_operator_operation_node(operator_stack, node_stack);
					operator_stack.push(type);
				} else {
					while (!operator_stack.empty() && 0 < operator_precedence_compare(operator_stack.top(), type))
						gen_top_operator_operation_node(operator_stack, node_stack);
					operator_stack.push(type);
				}
			}
			++current;
			previous_is_operator = true;
		}
			break;
			// Digit
		case TK_DEC_DIGIT:
		case TK_HEX_DIGIT:
		case TK_OCT_DIGIT: {
			std::shared_ptr<LiteralNode> literal_node(new LiteralNode());
			literal_node->setType(ND_LITERAL);
			if (!literal_node->setValue((*current)->getName().c_str()))
				return nullptr;
			std::shared_ptr<Node> node = std::dynamic_pointer_cast<Node>(literal_node);
			node_stack.push(node);
			++current;
			previous_is_operator = false;
		}
			break;
		case TK_IDENTIFIER: {
			std::string const &name = (*(current++))->getName();
			// function
			if (expect(current, end, "(")) {
				std::shared_ptr<FunNode> fun_node(new FunNode(nullptr, ND_FUN, name.c_str()));
				node_stack.push(std::dynamic_pointer_cast<Node>(fun_node));
				if (!expect(current, end, ")")) {
					do {
						std::shared_ptr<Node> node = gen_expr_node(current, end);
						if (!node)
							return nullptr;
						node->setParent(std::dynamic_pointer_cast<Node>(fun_node));
						fun_node->add_parameter(node);
					} while (expect(current, end, ","));
					if (!expect(current, end, ")"))
						return nullptr;
				} else {
					// No parameter
				}
			} else { // variable
				std::shared_ptr<VarNode> var_node(new VarNode(nullptr, ND_VAR, name.c_str()));
				node_stack.push(std::dynamic_pointer_cast<Node>(var_node));
			}
			previous_is_operator = false;
		}
			break;
		case TK_SYMBOL:
			// Parenthesis
			if ((*current)->getName() == "(") {
				++current;
				std::shared_ptr<Node> node = gen_expr_node(current, end);
				if (!node)
					return nullptr;
				std::shared_ptr<ParenthesisNode> parenthesis_node(new ParenthesisNode(nullptr, ND_PARENTHESIS, node));
				node->setParent(std::dynamic_pointer_cast<Node>(parenthesis_node));
				node_stack.push(std::dynamic_pointer_cast<Node>(parenthesis_node));
				if (!expect(current, end, ")"))
					return nullptr;
				previous_is_operator = false;
				break;
			}
			// Array
			if ((*current)->getName() == "{") {
				++current;
				std::shared_ptr<ArrayNode> array_node(new ArrayNode(nullptr, ND_ARRAY));
				node_stack.push(std::dynamic_pointer_cast<Node>(array_node));
				if (!expect(current, end, "}")) { // have element
					do {
						std::shared_ptr<Node> node = gen_expr_node(current, end);
						if (!node)
							return nullptr;
						node->setParent(std::dynamic_pointer_cast<Node>(array_node));
						array_node->add_array(node);
					} while (expect(current, end, ","));
					if (!expect(current, end, "}"))
						return nullptr;
				} else { // not have element

				}
				previous_is_operator = false;
				break;
			}
			// Array access
			if ((*current)->getName() == "[") {
				++current;
				if (node_stack.empty())
					return nullptr;
				std::shared_ptr<ArrayAccessNode> array_access_node;
				std::shared_ptr<Node> &top_node = node_stack.top();
				if (ND_ARRAY_ACCESS != top_node->getType()) {
					array_access_node = std::shared_ptr<ArrayAccessNode>(new ArrayAccessNode(nullptr, ND_ARRAY_ACCESS, top_node));
					top_node->setParent(std::dynamic_pointer_cast<Node>(array_access_node));
					std::shared_ptr<Node> node = std::dynamic_pointer_cast<Node>(array_access_node);
					node_stack.pop();
					node_stack.push(node);
				} else {
					array_access_node = std::dynamic_pointer_cast<ArrayAccessNode>(top_node);
				}
				std::shared_ptr<Node> node = gen_expr_node(current, end);
				if (!node)
					return nullptr;
				node->setParent(std::dynamic_pointer_cast<Node>(array_access_node));
				array_access_node->add_index(node);
				if (!expect(current, end, "]"))
					return nullptr;
				previous_is_operator = false;
				break;
			}
			/* no break */
		default:
			goto for_end;
		}
		// End of input str
		if (end == current)
			break;
	}
	for_end: // for loop end

	while (!operator_stack.empty())
		if (!gen_top_operator_operation_node(operator_stack, node_stack))
			return nullptr;
	if (1 != node_stack.size())
		return nullptr;
	std::shared_ptr<Node> node = node_stack.top();
	node_stack.pop();
	return node;
}

std::shared_ptr<TypeNode> gen_type_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {
	TK_TYPE const type = (*current)->getType();
	std::shared_ptr<TypeNode> _type(new TypeNode(nullptr, ND_TYPE_, TK_INT == type ? VA_INT : VA_VOID));
	++current;
	while (expect(current, end, "[")) {
		if (expect(current, end, "]")) { // empty dimension [][] set to -1;
			std::shared_ptr<LiteralNode> literal_node(new LiteralNode(nullptr, ND_LITERAL, -1));
			std::shared_ptr<Node> node = std::dynamic_pointer_cast<Node>(literal_node);
			_type->add_dimension(node);
			node->setParent(std::dynamic_pointer_cast<Node>(_type));
		} else {
			std::shared_ptr<Node> node = gen_expr_node(current, end);
			_type->add_dimension(node);
			node->setParent(std::dynamic_pointer_cast<Node>(_type));
			if (!expect(current, end, "]"))
				return nullptr;
		}
	}
	return _type;
}

std::shared_ptr<VarTypeNode> gen_var_type_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {
	std::string const &name = (*current)->getName();
	std::shared_ptr<VarTypeNode> var_type_node(new VarTypeNode(nullptr, ND_VAR_TYPE, name.c_str()));
	++current;
	while (expect(current, end, "[")) {
		if (expect(current, end, "]")) { // empty dimension [][] set to -1;
			std::shared_ptr<LiteralNode> literal_node(new LiteralNode(nullptr, ND_LITERAL, -1));
			std::shared_ptr<Node> node = std::dynamic_pointer_cast<Node>(literal_node);
			var_type_node->add_dimension(node);
			node->setParent(std::dynamic_pointer_cast<Node>(var_type_node));
		} else {
			std::shared_ptr<Node> node = gen_expr_node(current, end);
			var_type_node->add_dimension(node);
			node->setParent(std::dynamic_pointer_cast<Node>(var_type_node));
			if (!expect(current, end, "]"))
				return nullptr;
		}
	}
	return var_type_node;
}

/* this use for function parameter */
std::shared_ptr<VarDefNode> gen_single_variable_definition_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {

	std::shared_ptr<TypeNode> _type = gen_type_node(current, end);
	if (!_type)
		return nullptr;
	std::shared_ptr<VarDefNode> var_def_node(new VarDefNode(nullptr, ND_VAR_DEF, "", _type, false));
	std::shared_ptr<VarTypeNode> var_type = gen_var_type_node(current, end);
	if (!var_type)
		return nullptr;
	std::shared_ptr<Node> var_init_value;
	if (expect(current, end, "=")) { // Have init value
		var_init_value = gen_expr_node(current, end);
		if (!var_init_value)
			return nullptr;
	}
	var_def_node->add_var(var_type, var_init_value);
	var_type->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
	if (var_init_value)
		var_init_value->setParent(std::dynamic_pointer_cast<Node>(var_def_node));

	return var_def_node;
}

std::shared_ptr<VarDefNode> gen_multi_variable_definition_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {
	std::shared_ptr<TypeNode> _type = gen_type_node(current, end);
	if (!_type)
		return nullptr;
	std::shared_ptr<VarDefNode> var_def_node(new VarDefNode(nullptr, ND_VAR_DEF, "", _type, false));
	while (current != end) {
		std::shared_ptr<VarTypeNode> var_type = gen_var_type_node(current, end);
		if (!var_type)
			return nullptr;
		std::shared_ptr<Node> var_init_value;
		if (expect(current, end, "=")) { // Have init value
			var_init_value = gen_expr_node(current, end);
			if (!var_init_value)
				return nullptr;
		}
		var_def_node->add_var(var_type, var_init_value);
		var_type->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
		if (var_init_value)
			var_init_value->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
		if (expect(current, end, ","))
			continue; //move to next
		if (expect(current, end, ";"))
			break; //end of definition
		return nullptr;
	}
	return var_def_node;
}

std::shared_ptr<BlockNode> gen_block_or_single_line_block(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {
	std::shared_ptr<BlockNode> block_node;
	if (expect(current, end, "{")) { //block body
		block_node = gen_block_node(current, end);
		if (!block_node)
			return nullptr;
		if (!expect(current, end, "}"))
			return nullptr;
	} else { // single line body
		bool is_reach_end = false;
		std::shared_ptr<Node> child_node = gen_block_child_node(current, end, is_reach_end);
		if (is_reach_end)
			return nullptr;
		if (!child_node)
			return nullptr;
		block_node = std::shared_ptr<BlockNode>(new BlockNode(nullptr, ND_BLOCK, child_node));
		child_node->setParent(std::dynamic_pointer_cast<Node>(block_node));
	}
	return block_node;
}

std::shared_ptr<IfElseNode> gen_if_else_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {
	if (!expect(current, end, "if"))
		return nullptr;
	if (!expect(current, end, "("))
		return nullptr;
	std::shared_ptr<Node> condition = gen_expr_node(current, end);
	if (!condition)
		return nullptr;
	if (!expect(current, end, ")"))
		return nullptr;
	std::shared_ptr<BlockNode> if_body = gen_block_or_single_line_block(current, end);
	if (!if_body)
		return nullptr;
	std::shared_ptr<BlockNode> else_body;
	if (expect(current, end, "else")) {  // else block
		else_body = gen_block_or_single_line_block(current, end);
		if (!else_body)
			return nullptr;
	} else {  // Not have else block
	}
	std::shared_ptr<IfElseNode> if_else_node(new IfElseNode(nullptr, ND_IF_ELSE, condition, if_body, else_body));
	condition->setParent(std::dynamic_pointer_cast<Node>(if_else_node));
	if_body->setParent(std::dynamic_pointer_cast<Node>(if_else_node));
	if (else_body)
		else_body->setParent(std::dynamic_pointer_cast<Node>(if_else_node));
	return if_else_node;
}

std::shared_ptr<Node> gen_block_child_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end, bool &is_reach_end) {
	is_reach_end = false;
	if (current == end) {
		is_reach_end = true;
		return nullptr;
	}
	TK_TYPE type = (*current)->getType();
	switch (type) {
	case TK_SYMBOL: {
		std::string const &name = (*current)->getName();
		if ("(" != name/*  && "{" != name  */) {
			switch (*name.c_str()) {
			case ';': //empty expression
			{
				std::shared_ptr<StmtNode> stmt_node(new StmtNode(nullptr, ND_STMT, nullptr));
				++current;
				return std::dynamic_pointer_cast<Node>(stmt_node);
			}
			case '{': //block
			{
				++current;
				std::shared_ptr<BlockNode> _block_node = gen_block_node(current, end);
				if (!_block_node)
					return nullptr;
//				block_node->add_child(std::dynamic_pointer_cast<Node>(_block_node));
//				_block_node->setParent(std::dynamic_pointer_cast<Node>(block_node));
				if (!expect(current, end, "}"))
					return nullptr;
				return std::dynamic_pointer_cast<Node>(_block_node);
			}
			default:
//				goto while_end;
				is_reach_end = true;
				return nullptr;
			}
		}
	}
		/* no break */
	case TK_IDENTIFIER: //function name or variable name
	case TK_OPERATOR:
	case TK_DEC_DIGIT:
	case TK_HEX_DIGIT:
	case TK_OCT_DIGIT: {
		// expression statement
		std::shared_ptr<Node> node = gen_expr_node(current, end);
		if (!node)
			return nullptr;
		std::shared_ptr<StmtNode> stmt_node(new StmtNode(nullptr, ND_STMT, node));
		node->setParent(std::dynamic_pointer_cast<Node>(stmt_node));
//		block_node->add_child(std::dynamic_pointer_cast<Node>(stmt_node));
//		stmt_node->setParent(std::dynamic_pointer_cast<Node>(block_node));
		if (!expect(current, end, ";"))
			return nullptr;
		return std::dynamic_pointer_cast<Node>(stmt_node);
	}
	case TK_CONST: {
		++current;
		std::shared_ptr<VarDefNode> var_def_node = gen_multi_variable_definition_node(current, end);
		if (!var_def_node)
			return nullptr;
		var_def_node->setIsConst(true);
//		block_node->add_child(std::dynamic_pointer_cast<Node>(var_def_node));
//		var_def_node->setParent(std::dynamic_pointer_cast<Node>(block_node));
		return std::dynamic_pointer_cast<Node>(var_def_node);
	}
	case TK_INT:
	case TK_VOID: {
		// variable & function definition
		std::shared_ptr<TypeNode> _type = gen_type_node(current, end);
		if (!_type)
			return nullptr;
		TK_TYPE type = (*current)->getType();
		if (TK_IDENTIFIER != type)
			return nullptr;
		std::string const &name = (*current)->getName();
		// function definition / declaration
		++current;
		if (expect(current, end, "(")) {
			std::shared_ptr<FunDefNode> fun_def_node(new FunDefNode());
			fun_def_node->setType(ND_FUN_DEF);
			fun_def_node->setReturnType(_type);
			fun_def_node->setName(name);
			_type->setParent(fun_def_node);

//			block_node->add_child(std::dynamic_pointer_cast<Node>(fun_def_node));
//			fun_def_node->setParent(std::dynamic_pointer_cast<Node>(block_node));
			// Function parameters
			if (!expect(current, end, ")")) {
				do {
					std::shared_ptr<VarDefNode> var_def_node = gen_single_variable_definition_node(current, end);
					if (!var_def_node)
						return nullptr;
					fun_def_node->add_parameter(var_def_node);
					var_def_node->setParent(std::dynamic_pointer_cast<Node>(fun_def_node));
				} while (expect(current, end, ","));
				if (!expect(current, end, ")"))
					return nullptr;
			} else {
				// No parameter
			}
			// Function body
			if (!expect(current, end, ";")) {
				if (!expect(current, end, "{"))
					return nullptr;
				std::shared_ptr<BlockNode> block_node = gen_block_node(current, end);
				if (!block_node)
					return nullptr;
				fun_def_node->setBody(block_node);
				block_node->setParent(std::dynamic_pointer_cast<Node>(fun_def_node));
				if (!expect(current, end, "}"))
					return nullptr;
			}
			return std::dynamic_pointer_cast<Node>(fun_def_node);
		} else { // variable definition
			--current;
			std::shared_ptr<VarDefNode> var_def_node(new VarDefNode(nullptr, ND_VAR_DEF, "", _type, false));
			while (current != end) {
				std::shared_ptr<VarTypeNode> var_type = gen_var_type_node(current, end);
				if (!var_type)
					return nullptr;
				std::shared_ptr<Node> var_init_value;
				if (expect(current, end, "=")) { // Have init value
					var_init_value = gen_expr_node(current, end);
					if (!var_init_value)
						return nullptr;
				}
				var_def_node->add_var(var_type, var_init_value);
				var_type->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
				if (var_init_value)
					var_init_value->setParent(std::dynamic_pointer_cast<Node>(var_def_node));
				if (expect(current, end, ","))
					continue; //move to next
				if (expect(current, end, ";"))
					break; //end of definition
				return nullptr;
			}
			return std::dynamic_pointer_cast<Node>(var_def_node);
		}
	}
	case TK_IF: {
		std::shared_ptr<IfElseNode> if_else_node = gen_if_else_node(current, end);
		if (!if_else_node)
			return nullptr;
		return std::dynamic_pointer_cast<Node>(if_else_node);
	}
	case TK_WHILE: {
		++current;
		if (!expect(current, end, "("))
			return nullptr;
		std::shared_ptr<Node> condition = gen_expr_node(current, end);
		if (!condition)
			return nullptr;
		if (!expect(current, end, ")"))
			return nullptr;
		std::shared_ptr<BlockNode> body = gen_block_or_single_line_block(current, end);
		if (!body)
			return nullptr;
		std::shared_ptr<WhileNode> while_node(new WhileNode(nullptr, ND_WHILE, condition, body));
		condition->setParent(std::dynamic_pointer_cast<Node>(while_node));
		body->setParent(std::dynamic_pointer_cast<Node>(while_node));
		return std::dynamic_pointer_cast<Node>(while_node);
	}
	case TK_CONTINUE: {
		++current;
		if (!expect(current, end, ";"))
			return nullptr;
		return std::dynamic_pointer_cast<Node>(std::shared_ptr<ContinueNode>(new ContinueNode(nullptr, ND_CONTINUE)));
	}
	case TK_BREAK: {
		++current;
		if (!expect(current, end, ";"))
			return nullptr;
		return std::dynamic_pointer_cast<Node>(std::shared_ptr<BreakNode>(new BreakNode(nullptr, ND_BREAK)));
	}
	case TK_RETURN: {
		++current;
		std::shared_ptr<Node> return_value;
		if (!expect(current, end, ";")) { // Have return value
			return_value = gen_expr_node(current, end);
			if (!return_value)
				return nullptr;
			if (!expect(current, end, ";"))
				return nullptr;
		}
		std::shared_ptr<ReturnNode> return_node(new ReturnNode(nullptr, ND_RETURN, return_value));
		if (return_value)
			return_value->setParent(return_node);
		return std::dynamic_pointer_cast<Node>(return_node);
	}

	case TK_ELSE:
	case TK_UNKNOWN:
	default:
		// This is an error!
		return nullptr;
	}
// Never reached
	return nullptr;
}

std::shared_ptr<BlockNode> gen_block_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end) {
	std::shared_ptr<BlockNode> block_node(new BlockNode(nullptr, ND_BLOCK));
	while (end != current) {
		bool is_reach_end = false;
		std::shared_ptr<Node> child_node = gen_block_child_node(current, end, is_reach_end);
		if (is_reach_end)
			goto while_end;
		if (!child_node)
			return nullptr;
		block_node->add_child(std::dynamic_pointer_cast<Node>(child_node));
		child_node->setParent(std::dynamic_pointer_cast<Node>(block_node));
	}
	while_end: //end of while
	return block_node;
}

// Every new node will set the parent type

std::shared_ptr<BlockNode> parse(std::vector<std::shared_ptr<Token>>const &tokens) {
	std::vector<std::shared_ptr<Token>>::const_iterator begin = tokens.begin(), end = tokens.end(), current = begin;
	std::shared_ptr<BlockNode> root_block = gen_block_node(current, end);
	if (!root_block)
		return nullptr;
	if (current != end)
		return nullptr;
	return root_block;
}

void build_array(std::vector<std::shared_ptr<Node> >const &array, std::ostringstream &oss) {
	oss << "[";
	for (std::vector<std::shared_ptr<Node> >::const_iterator it = array.begin(), end = array.end(); it != end; ++it) {
		dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
		oss << ",";
	}
	oss << "]";
}

bool dump_node2json(std::shared_ptr<Node> const &node, std::ostringstream &oss) {
	ND_TYPE type = node->getType();
	oss << "{";
	switch (type) {
	case ND_TYPE_: {
		std::shared_ptr<TypeNode> const &_node = std::dynamic_pointer_cast<TypeNode>(node);
		oss << "type:" << "\"ND_TYPE_\"" << ",";
		oss << "variable_type:" << _node->getVariableType() << ",";
		std::vector<std::shared_ptr<Node>> const &dimensions = _node->getDimensions();
		oss << "dimenssions:";
		build_array(dimensions, oss);
		oss << ",";
	}
		break;
	case ND_VAR_TYPE: {
		std::shared_ptr<VarTypeNode> const &_node = std::dynamic_pointer_cast<VarTypeNode>(node);
		oss << "type:" << "\"ND_VAR_TYPE\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";
		std::vector<std::shared_ptr<Node>> const &dimensions = _node->getDimensions();
		oss << "dimenssions:";
		build_array(dimensions, oss);
		oss << ",";

	}
		break;
	case ND_DEF: {
		std::shared_ptr<DefNode> const &_node = std::dynamic_pointer_cast<DefNode>(node);
		oss << "type:" << "\"ND_DEF\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";

	}
		break;
	case ND_VAR_DEF: {
		std::shared_ptr<VarDefNode> const &_node = std::dynamic_pointer_cast<VarDefNode>(node);
		oss << "type:" << "\"ND_VAR_DEF\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";
		oss << "variable_type:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getVariableType()), oss);
		oss << ",";
		std::vector<std::shared_ptr<VarTypeNode> >const &var_type = _node->getVarType();
		oss << "var_type:" << "[";
		for (std::vector<std::shared_ptr<VarTypeNode> >::const_iterator it = var_type.begin(), end = var_type.end(); it != end; ++it) {
			dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
			oss << ",";
		}
		oss << "]" << ",";
		std::vector<std::shared_ptr<Node> >const &var_init_value = _node->getVarInitValue();
		oss << "var_init_value:" << "[";
		for (std::vector<std::shared_ptr<Node> >::const_iterator it = var_init_value.begin(), end = var_init_value.end(); it != end; ++it) {
			if (*it)
				dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
			oss << ",";
		}
		oss << "]" << ",";
		oss << "is_const:" << _node->isIsConst() << ",";

	}
		break;
	case ND_FUN_DEF: {
		std::shared_ptr<FunDefNode> const &_node = std::dynamic_pointer_cast<FunDefNode>(node);
		oss << "type:" << "\"ND_FUN_DEF\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";
		oss << "return_type:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getReturnType()), oss);
		oss << ",";
		std::vector<std::shared_ptr<VarDefNode> >const &parameters = _node->getParameters();
		oss << "parameters:" << "[";
		for (std::vector<std::shared_ptr<VarDefNode> >::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it) {
			dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
			oss << ",";
		}
		oss << "]" << ",";
		oss << "body:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getBody()), oss);
		oss << ",";
	}
		break;
	case ND_REF: {
		std::shared_ptr<RefNode> const &_node = std::dynamic_pointer_cast<RefNode>(node);
		oss << "type:" << "\"ND_REF\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";

	}
		break;
	case ND_VAR: {
		std::shared_ptr<VarNode> const &_node = std::dynamic_pointer_cast<VarNode>(node);
		oss << "type:" << "\"ND_VAR\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";

	}
		break;
	case ND_FUN: {
		std::shared_ptr<FunNode> const &_node = std::dynamic_pointer_cast<FunNode>(node);
		oss << "type:" << "\"ND_FUN\"" << ",";
		oss << "name:" << "\"" << _node->getName() << "\"" << ",";
		std::vector<std::shared_ptr<Node> >const &parameters = _node->getParameters();
		oss << "parameters:" << "[";
		for (std::vector<std::shared_ptr<Node> >::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it) {
			dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
			oss << ",";
		}
		oss << "]" << ",";

	}
		break;
	case ND_STMT: {
		std::shared_ptr<StmtNode> const &_node = std::dynamic_pointer_cast<StmtNode>(node);
		oss << "type:" << "\"ND_STMT\"" << ",";
		std::shared_ptr<Node> stmt = _node->getStmt();
		if (stmt) {
			oss << "stmt:";
			dump_node2json(std::dynamic_pointer_cast<Node>(stmt), oss);
			oss << ",";
		}
	}
		break;
	case ND_PARENTHESIS: {
		std::shared_ptr<ParenthesisNode> const &_node = std::dynamic_pointer_cast<ParenthesisNode>(node);
		oss << "type:" << "\"ND_PARENTHESIS\"" << ",";
		oss << "node:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getNode()), oss);
		oss << ",";

	}
		break;
	case ND_LITERAL: {
		std::shared_ptr<LiteralNode> const &_node = std::dynamic_pointer_cast<LiteralNode>(node);
		oss << "type:" << "\"ND_LITERAL\"" << ",";
		oss << "value:" << _node->getValue() << ",";

	}
		break;
	case ND_ARRAY: {
		std::shared_ptr<ArrayNode> const &_node = std::dynamic_pointer_cast<ArrayNode>(node);
		oss << "type:" << "\"ND_ARRAY\"" << ",";
		std::vector<std::shared_ptr<Node> >const &array = _node->getArray();
		oss << "array:" << "[";

		for (std::vector<std::shared_ptr<Node> >::const_iterator it = array.begin(), end = array.end(); it != end; ++it) {
			dump_node2json(std::dynamic_pointer_cast<Node>(*it), oss);
			oss << ",";
		}
		oss << "]" << ",";

	}
		break;
	case ND_ARRAY_ACCESS: {
		std::shared_ptr<ArrayAccessNode> const &_node = std::dynamic_pointer_cast<ArrayAccessNode>(node);
		oss << "type:" << "\"ND_ARRAY_ACCESS\"" << ",";
		std::vector<std::shared_ptr<Node> >const &indexes = _node->getIndexes();
		oss << "indexes:";
		build_array(indexes, oss);
		oss << ",";
		oss << "target:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getTarget()), oss);
		oss << ",";
	}
		break;
	case ND_OPERATION: {
		std::shared_ptr<OperationNode> const &_node = std::dynamic_pointer_cast<OperationNode>(node);
		oss << "type:" << "\"ND_OPERATION\"" << ",";
		OP_TYPE operator_type = _node->getOperatorType();
		oss << "operator_type:" << "\"";
		switch (operator_type) {
		case OP_UNARY_PLUS:
			oss << "/* +  */";
			break;
		case OP_UNARY_MINUS:
			oss << "/* -  */";
			break;
		case OP_LOGICAL_NOT:
			oss << "/* !  */";
			break;
		case OP_MUL:
			oss << "/* *  */";
			break;
		case OP_DIV:
			oss << "/* /  */";
			break;
		case OP_MOD:
			oss << "/* %  */";
			break;
		case OP_ADD:
			oss << "/* +  */";
			break;
		case OP_SUB:
			oss << "/* -  */";
			break;
		case OP_GREATER:
			oss << "/* >  */";
			break;
		case OP_LESS:
			oss << "/* <  */";
			break;
		case OP_GREATER_EQUAL:
			oss << "/* >= */";
			break;
		case OP_LESS_EQUAL:
			oss << "/* <= */";
			break;
		case OP_EQUAL:
			oss << "/* == */";
			break;
		case OP_NOT_EQUAL:
			oss << "/* != */";
			break;
		case OP_LOGICAL_AND:
			oss << "/* && */";
			break;
		case OP_LOGICAL_OR:
			oss << "/* || */";
			break;
		case OP_ASSIGNMENT:
			oss << "/* =  */";
			break;
		default:
			oss << "This is an error!";
			break;
		}
		oss << "\"" << ",";
		std::vector<std::shared_ptr<Node> >const &parameters = _node->getParameters();
		oss << "parameters:";
		build_array(parameters, oss);
		oss << ",";
	}
		break;
	case ND_IF_ELSE: {
		std::shared_ptr<IfElseNode> const &_node = std::dynamic_pointer_cast<IfElseNode>(node);
		oss << "type:" << "\"ND_IF_ELSE\"" << ",";
		oss << "condition:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getCondition()), oss);
		oss << ",";
		oss << "if_body:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getIfBody()), oss);
		oss << ",";
		if (_node->getElseBody()) {
			oss << "else_body:";
			dump_node2json(std::dynamic_pointer_cast<Node>(_node->getElseBody()), oss);
			oss << ",";
		}
	}
		break;
	case ND_WHILE: {
		std::shared_ptr<WhileNode> const &_node = std::dynamic_pointer_cast<WhileNode>(node);
		oss << "type:" << "\"ND_WHILE\"" << ",";
		oss << "condition:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getCondition()), oss);
		oss << ",";
		oss << "body:";
		dump_node2json(std::dynamic_pointer_cast<Node>(_node->getBody()), oss);
		oss << ",";
	}
		break;
	case ND_CONTINUE: {
		std::shared_ptr<ContinueNode> const &_node = std::dynamic_pointer_cast<ContinueNode>(node);
		oss << "type:" << "\"ND_CONTINUE\"" << ",";

	}
		break;
	case ND_BREAK: {
		std::shared_ptr<BreakNode> const &_node = std::dynamic_pointer_cast<BreakNode>(node);
		oss << "type:" << "\"ND_BREAK\"" << ",";

	}
		break;
	case ND_RETURN: {
		std::shared_ptr<ReturnNode> const &_node = std::dynamic_pointer_cast<ReturnNode>(node);
		oss << "type:" << "\"ND_RETURN\"" << ",";
		if (_node->getReturnValue()) {
			oss << "return_value:";
			dump_node2json(std::dynamic_pointer_cast<Node>(_node->getReturnValue()), oss);
			oss << ",";
		}
	}
		break;
	case ND_BLOCK: {
		std::shared_ptr<BlockNode> const &_node = std::dynamic_pointer_cast<BlockNode>(node);
		oss << "type:" << "\"ND_BLOCK\"" << ",";
		std::vector<std::shared_ptr<Node> >const &children = _node->getChildren();
		oss << "children:";
		build_array(children, oss);
		oss << ",";
	}
		break;
	case ND_UNKNOWN:
	default:
		// This is an error!!!
		return false;
	}
	oss << "}";
	return true;
}

