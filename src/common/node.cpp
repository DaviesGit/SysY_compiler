/*
 * Node.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#include <cstdlib>
#include <climits>
#include <algorithm>

#include "node.h"

//########################## Node ##########################

Node::Node() : Node(nullptr, ND_UNKNOWN)
{
}
Node::Node(std::shared_ptr<Node> const &parent, ND_TYPE type) : parent(parent), type(type)
{
}

const std::shared_ptr<Node> &Node::getParent() const
{
	return parent;
}

void Node::setParent(const std::shared_ptr<Node> &parent)
{
	this->parent = parent;
}

ND_TYPE Node::getType() const
{
	return type;
}

void Node::setType(ND_TYPE type)
{
	this->type = type;
}

//########################## TypeNode ##########################

TypeNode::TypeNode() : Node(), variable_type(VA_UNKNOWN), dimensions()
{
}

std::vector<std::shared_ptr<Node>> &TypeNode::getDimensions()
{
	return dimensions;
}

void TypeNode::setDimensions(const std::vector<std::shared_ptr<Node>> &dimensions)
{
	this->dimensions = dimensions;
}

void TypeNode::add_dimension(std::shared_ptr<Node> const &dimension)
{
	this->dimensions.push_back(dimension);
}

VA_TYPE TypeNode::getVariableType() const
{
	return variable_type;
}

void TypeNode::setVariableType(VA_TYPE variableType)
{
	variable_type = variableType;
}

//########################## VarTypeNode ##########################

VarTypeNode::VarTypeNode() : Node(), name(), dimensions()
{
}

std::vector<std::shared_ptr<Node>> &VarTypeNode::getDimensions()
{
	return dimensions;
}

void VarTypeNode::setDimensions(const std::vector<std::shared_ptr<Node>> &dimensions)
{
	this->dimensions = dimensions;
}

void VarTypeNode::add_dimension(std::shared_ptr<Node> const &dimension)
{
	this->dimensions.push_back(dimension);
}

const std::string &VarTypeNode::getName() const
{
	return name;
}

void VarTypeNode::setName(const std::string &name)
{
	this->name = name;
}

//########################## DefNode ##########################

DefNode::DefNode() : Node(), name()
{
}
DefNode::DefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name) : Node(parent, type), name(name)
{
}

const std::string &DefNode::getName() const
{
	return name;
}

void DefNode::setName(const std::string &name)
{
	this->name = name;
}

//########################## VarDefNode ##########################

VarDefNode::VarDefNode() : DefNode(), variable_type(nullptr), is_const(false), var_type(), var_init_value(), block_id(-1), is_global(false), var_offset()
{
}

VarDefNode::VarDefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name, std::shared_ptr<TypeNode> const &variable_type, bool is_const) : DefNode(parent, type, name), variable_type(variable_type), is_const(is_const), var_type(), var_init_value(), block_id(-1), is_global(false), var_offset()
{
}

bool VarDefNode::isIsConst() const
{
	return is_const;
}

void VarDefNode::setIsConst(bool isConst)
{
	is_const = isConst;
}

std::shared_ptr<TypeNode> &VarDefNode::getVariableType()
{
	return variable_type;
}

void VarDefNode::setVariableType(const std::shared_ptr<TypeNode> &variableType)
{
	variable_type = variableType;
}

std::vector<std::shared_ptr<Node>> &VarDefNode::getVarInitValue()
{
	return var_init_value;
}

void VarDefNode::setVarInitValue(const std::vector<std::shared_ptr<Node>> &varInitValue)
{
	var_init_value = varInitValue;
}

std::vector<std::shared_ptr<VarTypeNode>> &VarDefNode::getVarType()
{
	return var_type;
}

void VarDefNode::setVarType(const std::vector<std::shared_ptr<VarTypeNode>> &varType)
{
	var_type = varType;
}

void VarDefNode::add_var(std::shared_ptr<VarTypeNode> var_type, std::shared_ptr<Node> var_init_value)
{
	this->var_type.push_back(var_type);
	this->var_init_value.push_back(var_init_value);
}

int VarDefNode::getBlockId() const
{
	return block_id;
}

void VarDefNode::setBlockId(int blockId)
{
	block_id = blockId;
}
bool VarDefNode::isIsGlobal() const
{
	return is_global;
}

void VarDefNode::setIsGlobal(bool isGlobal)
{
	is_global = isGlobal;
}

const std::vector<int> &VarDefNode::getVarOffset() const
{
	return var_offset;
}

void VarDefNode::setVarOffset(const std::vector<int> &varOffset)
{
	var_offset = varOffset;
}

void VarDefNode::add_var_offset(const int var_offset)
{
	this->var_offset.push_back(var_offset);
}

//########################## FunDefNode ##########################

FunDefNode::FunDefNode() : DefNode(), return_type(nullptr), body(nullptr), parameters(), block_id(-1), return_label_id(-1), local_var_total_size(-1)
{
}

const std::shared_ptr<BlockNode> &FunDefNode::getBody() const
{
	return body;
}

void FunDefNode::setBody(const std::shared_ptr<BlockNode> &body)
{
	this->body = body;
}

std::vector<std::shared_ptr<VarDefNode>> &FunDefNode::getParameters()
{
	return parameters;
}

void FunDefNode::setParameters(const std::vector<std::shared_ptr<VarDefNode>> &parameters)
{
	this->parameters = parameters;
}

void FunDefNode::add_parameter(std::shared_ptr<VarDefNode> const &parameter)
{
	this->parameters.push_back(parameter);
}

const std::shared_ptr<TypeNode> &FunDefNode::getReturnType() const
{
	return return_type;
}

void FunDefNode::setReturnType(const std::shared_ptr<TypeNode> &returnType)
{
	return_type = returnType;
}

int FunDefNode::getBlockId() const
{
	return block_id;
}

void FunDefNode::setBlockId(int blockId)
{
	block_id = blockId;
}

int FunDefNode::getLocalVarTotalSize() const
{
	return local_var_total_size;
}

void FunDefNode::setLocalVarTotalSize(int localVarTotalSize)
{
	local_var_total_size = localVarTotalSize;
}

int FunDefNode::getReturnLabelId() const
{
	return return_label_id;
}

void FunDefNode::setReturnLabelId(int returnLabelId)
{
	return_label_id = returnLabelId;
}

//########################## RefNode ##########################

RefNode::RefNode() : Node(), name()
{
}
RefNode::RefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name) : Node(parent, type), name(name)
{
}

const std::string &RefNode::getName() const
{
	return name;
}

void RefNode::setName(const std::string &name)
{
	this->name = name;
}

//########################## VarNode ##########################

VarNode::VarNode() : RefNode(), target_var(nullptr)
{
}
VarNode::VarNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name) : RefNode(parent, type, name), target_var(nullptr)
{
}

const std::shared_ptr<VarTypeNode> &VarNode::getTargetVar() const
{
	return target_var;
}

void VarNode::setTargetVar(const std::shared_ptr<VarTypeNode> &targetVar)
{
	target_var = targetVar;
}

//########################## FunNode ##########################

FunNode::FunNode() : RefNode(), parameters(), is_system_fun(false), target_fun(nullptr)
{
}

std::vector<std::shared_ptr<Node>> &FunNode::getParameters()
{
	return parameters;
}

void FunNode::setParameters(const std::vector<std::shared_ptr<Node>> &parameters)
{
	this->parameters = parameters;
}

void FunNode::add_parameter(std::shared_ptr<Node> const &parameter)
{
	this->parameters.push_back(parameter);
}

bool FunNode::isIsSystemFun() const
{
	return is_system_fun;
}

void FunNode::setIsSystemFun(bool isSystemFun)
{
	is_system_fun = isSystemFun;
}

const std::shared_ptr<FunDefNode> &FunNode::getTargetFun() const
{
	return target_fun;
}

void FunNode::setTargetFun(const std::shared_ptr<FunDefNode> &targetFun)
{
	target_fun = targetFun;
}

//########################## StmtNode ##########################

StmtNode::StmtNode() : stmt(nullptr)
{
}
StmtNode::StmtNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &stmt) : Node(parent, type), stmt(stmt)
{
}

std::shared_ptr<Node> &StmtNode::getStmt()
{
	return stmt;
}

void StmtNode::setStmt(const std::shared_ptr<Node> &stmt)
{
	this->stmt = stmt;
}

//########################## ParenthesisNode ##########################

ParenthesisNode::ParenthesisNode() : node(nullptr)
{
}
ParenthesisNode::ParenthesisNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &node) : Node(parent, type), node(node)
{
}

std::shared_ptr<Node> &ParenthesisNode::getNode()
{
	return node;
}

void ParenthesisNode::setNode(const std::shared_ptr<Node> &node)
{
	this->node = node;
}

//########################## LiteralNode ##########################

LiteralNode::LiteralNode() : value(0)
{
}

LiteralNode::LiteralNode(std::shared_ptr<LiteralNode> const &literal_node) : Node(literal_node->getParent(), literal_node->getType()), value(literal_node->getValue())
{
}

LiteralNode::LiteralNode(std::shared_ptr<Node> const &parent, ND_TYPE type, long long value) : Node(parent, type), value(value)
{
}

long long LiteralNode::getValue() const
{
	return value;
}

void LiteralNode::setValue(long long value)
{
	this->value = value;
}

bool LiteralNode::setValue(char const *const value)
{
	if (!isdigit(*value))
		return false;
	char *end;
	long long _value = strtoll(value, &end, 0);
	if (0 != *end || _value == LLONG_MAX || _value == LLONG_MIN)
		return false;
	this->value = _value;
	return true;
}
//########################## ArrayNode ##########################

ArrayNode::ArrayNode() : array()
{
}

std::vector<std::shared_ptr<Node>> &ArrayNode::getArray()
{
	return array;
}

void ArrayNode::setArray(const std::vector<std::shared_ptr<Node>> &array)
{
	this->array = array;
}

void ArrayNode::add_array(std::shared_ptr<Node> const &element)
{
	this->array.push_back(element);
}

//########################## ArrayAccessNode ##########################

ArrayAccessNode::ArrayAccessNode() : target(nullptr), indexes()
{
}

std::vector<std::shared_ptr<Node>> &ArrayAccessNode::getIndexes()
{
	return indexes;
}

void ArrayAccessNode::setIndexes(const std::vector<std::shared_ptr<Node>> &indexes)
{
	this->indexes = indexes;
}

void ArrayAccessNode::add_index(std::shared_ptr<Node> const &index)
{
	this->indexes.push_back(index);
}

const std::shared_ptr<Node> &ArrayAccessNode::getTarget() const
{
	return target;
}

void ArrayAccessNode::setTarget(const std::shared_ptr<Node> &target)
{
	this->target = target;
}

//########################## OperationNode ##########################

OperationNode::OperationNode() : operator_type(OP_UNKNOWN), parameters()
{
}

OP_TYPE OperationNode::getOperatorType() const
{
	return operator_type;
}

void OperationNode::setOperatorType(OP_TYPE operatorType)
{
	operator_type = operatorType;
}

std::vector<std::shared_ptr<Node>> &OperationNode::getParameters()
{
	return parameters;
}

void OperationNode::setParameters(const std::vector<std::shared_ptr<Node>> &parameters)
{
	this->parameters = parameters;
}

void OperationNode::add_parameter(std::shared_ptr<Node> const &parameter)
{
	this->parameters.push_back(parameter);
}

void OperationNode::reverse_parameters(void)
{
	std::reverse(this->parameters.begin(), this->parameters.end());
}

//########################## IfElseNode ##########################

IfElseNode::IfElseNode() : condition(nullptr), if_body(nullptr), else_body(nullptr)
{
}
IfElseNode::IfElseNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &condition, std::shared_ptr<BlockNode> const &if_body,
					   std::shared_ptr<BlockNode> const &else_body) : Node(parent, type), condition(condition), if_body(if_body), else_body(else_body)
{
}

const std::shared_ptr<Node> &IfElseNode::getCondition() const
{
	return condition;
}

void IfElseNode::setCondition(const std::shared_ptr<Node> &condition)
{
	this->condition = condition;
}

const std::shared_ptr<BlockNode> &IfElseNode::getElseBody() const
{
	return else_body;
}

void IfElseNode::setElseBody(const std::shared_ptr<BlockNode> &elseBody)
{
	else_body = elseBody;
}

const std::shared_ptr<BlockNode> &IfElseNode::getIfBody() const
{
	return if_body;
}

void IfElseNode::setIfBody(const std::shared_ptr<BlockNode> &ifBody)
{
	if_body = ifBody;
}

//########################## WhileNode ##########################

WhileNode::WhileNode() : condition(nullptr), body(nullptr), continue_label_id(-1), break_label_id(-1)
{
}
WhileNode::WhileNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &condition, std::shared_ptr<BlockNode> const &body) : Node(parent, type), condition(condition), body(body), continue_label_id(-1), break_label_id(-1)
{
}

const std::shared_ptr<BlockNode> &WhileNode::getBody() const
{
	return body;
}

void WhileNode::setBody(const std::shared_ptr<BlockNode> &body)
{
	this->body = body;
}

const std::shared_ptr<Node> &WhileNode::getCondition() const
{
	return condition;
}

void WhileNode::setCondition(const std::shared_ptr<Node> &condition)
{
	this->condition = condition;
}

int WhileNode::getBreakLabelId() const
{
	return break_label_id;
}

void WhileNode::setBreakLabelId(int breakLabelId)
{
	break_label_id = breakLabelId;
}

int WhileNode::getContinueLabelId() const
{
	return continue_label_id;
}

void WhileNode::setContinueLabelId(int continueLabelId)
{
	continue_label_id = continueLabelId;
}

//########################## ContinueNode ##########################

ContinueNode::ContinueNode() : target(nullptr)
{
}
ContinueNode::ContinueNode(std::shared_ptr<Node> const &parent, ND_TYPE type) : Node(parent, type)
{
}

const std::shared_ptr<WhileNode> &ContinueNode::getTarget() const
{
	return target;
}

void ContinueNode::setTarget(const std::shared_ptr<WhileNode> &target)
{
	this->target = target;
}

//########################## BreakNode ##########################

BreakNode::BreakNode() : target(nullptr)
{
}
BreakNode::BreakNode(std::shared_ptr<Node> const &parent, ND_TYPE type) : Node(parent, type)
{
}

const std::shared_ptr<WhileNode> &BreakNode::getTarget() const
{
	return target;
}

void BreakNode::setTarget(const std::shared_ptr<WhileNode> &target)
{
	this->target = target;
}

//########################## ReturnNode ##########################

ReturnNode::ReturnNode() : return_value(nullptr), target(nullptr)
{
}
ReturnNode::ReturnNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &return_value) : Node(parent, type), return_value(return_value), target(nullptr)
{
}

const std::shared_ptr<Node> &ReturnNode::getReturnValue() const
{
	return return_value;
}

void ReturnNode::setReturnValue(const std::shared_ptr<Node> &returnValue)
{
	return_value = returnValue;
}

const std::shared_ptr<FunDefNode> &ReturnNode::getTarget() const
{
	return target;
}

void ReturnNode::setTarget(const std::shared_ptr<FunDefNode> &target)
{
	this->target = target;
}

//########################## BlockNode ##########################

BlockNode::BlockNode() : children(), id(-1)
{
}

std::vector<std::shared_ptr<Node>> &BlockNode::getChildren()
{
	return children;
}

void BlockNode::setChildren(const std::vector<std::shared_ptr<Node>> &children)
{
	this->children = children;
}

void BlockNode::add_child(std::shared_ptr<Node> const &child)
{
	this->children.push_back(child);
}

int BlockNode::getId() const
{
	return id;
}

void BlockNode::setId(int id)
{
	this->id = id;
}
