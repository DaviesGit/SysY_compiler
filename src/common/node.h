/*
 * Node.h
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#ifndef SRC_COMMON_NODE_H_
#define SRC_COMMON_NODE_H_

#include <string>
#include <vector>
#include <memory>

#include "type.h"

class Node;

class TypeNode;
class DefNode;
class is_const;
class RefNode;
class StmtNode;
class ParenthesisNode;
class LiteralNode;
class ArrayNode;
class OperationNode;
class IfElseNode;
class WhileNode;
class ContinueNode;
class BreakNode;
class ReturnNode;
class BlockNode;

class VarDefNode;
class is_const;
class FunDefNode;
class VarNode;
class FunNode;

class Node
{
private:
	std::shared_ptr<Node> parent;
	ND_TYPE type;

public:
	Node();
	virtual ~Node() = default;
	Node(std::shared_ptr<Node> const &parent, ND_TYPE type);
	const std::shared_ptr<Node> &getParent() const;
	void setParent(const std::shared_ptr<Node> &parent);
	ND_TYPE getType() const;
	void setType(ND_TYPE type);
};

class TypeNode : public Node
{
private:
	VA_TYPE variable_type;
	std::vector<std::shared_ptr<Node>> dimensions;

public:
	TypeNode();
	virtual ~TypeNode() = default;
	std::vector<std::shared_ptr<Node>> &getDimensions();
	void setDimensions(const std::vector<std::shared_ptr<Node>> &dimensions);
	void add_dimension(std::shared_ptr<Node> const &dimension);
	VA_TYPE getVariableType() const;
	void setVariableType(VA_TYPE variableType);
	template <class... Types>
	TypeNode(std::shared_ptr<Node> const &parent, ND_TYPE type, VA_TYPE variable_type, Types &&... args) : Node(parent, type), variable_type(variable_type), dimensions()
	{
		int dummy[sizeof...(Types)] = {(dimensions.push_back(args), 0)...};
	}
};

class VarTypeNode : public Node
{
private:
	std::string name;
	std::vector<std::shared_ptr<Node>> dimensions;

public:
	VarTypeNode();
	virtual ~VarTypeNode() = default;
	std::vector<std::shared_ptr<Node>> &getDimensions();
	void setDimensions(const std::vector<std::shared_ptr<Node>> &dimensions);
	void add_dimension(std::shared_ptr<Node> const &dimension);
	const std::string &getName() const;
	void setName(const std::string &name);
	template <class... Types>
	VarTypeNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name, Types &&... args) : Node(parent, type), name(name), dimensions()
	{
		int dummy[sizeof...(Types)] = {(dimensions.push_back(args), 0)...};
	}
};

class DefNode : public Node
{
private:
	std::string name;

public:
	DefNode();
	virtual ~DefNode() = default;
	DefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name);
	const std::string &getName() const;
	void setName(const std::string &name);
};

class VarDefNode : public DefNode
{
private:
	std::shared_ptr<TypeNode> variable_type;
	std::vector<std::shared_ptr<VarTypeNode>> var_type;
	std::vector<std::shared_ptr<Node>> var_init_value;
	bool is_const;
	int block_id;
	bool is_global;
	std::vector<int> var_offset;

public:
	VarDefNode();
	virtual ~VarDefNode() = default;
	VarDefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name, std::shared_ptr<TypeNode> const &variable_type, bool is_const);
	bool isIsConst() const;
	void setIsConst(bool isConst);
	std::shared_ptr<TypeNode> &getVariableType();
	void setVariableType(const std::shared_ptr<TypeNode> &variableType);
	std::vector<std::shared_ptr<Node>> &getVarInitValue();
	void setVarInitValue(const std::vector<std::shared_ptr<Node>> &varInitValue);
	std::vector<std::shared_ptr<VarTypeNode>> &getVarType();
	void setVarType(const std::vector<std::shared_ptr<VarTypeNode>> &varType);
	void add_var(std::shared_ptr<VarTypeNode> var_type, std::shared_ptr<Node> var_init_value);

	int getBlockId() const;
	void setBlockId(int blockId);
	bool isIsGlobal() const;
	void setIsGlobal(bool isGlobal);
	const std::vector<int> &getVarOffset() const;
	void setVarOffset(const std::vector<int> &varOffset);
	void add_var_offset(const int var_offset);
};

class FunDefNode : public DefNode
{
private:
	std::shared_ptr<TypeNode> return_type;
	std::shared_ptr<BlockNode> body;
	std::vector<std::shared_ptr<VarDefNode>> parameters;
	int block_id;
	int return_label_id;
	int local_var_total_size;

public:
	FunDefNode();
	virtual ~FunDefNode() = default;
	const std::shared_ptr<BlockNode> &getBody() const;
	void setBody(const std::shared_ptr<BlockNode> &body);
	std::vector<std::shared_ptr<VarDefNode>> &getParameters();
	void setParameters(const std::vector<std::shared_ptr<VarDefNode>> &parameters);
	void add_parameter(std::shared_ptr<VarDefNode> const &parameter);
	const std::shared_ptr<TypeNode> &getReturnType() const;
	void setReturnType(const std::shared_ptr<TypeNode> &returnType);
	template <class... Types>
	FunDefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name, std::shared_ptr<TypeNode> const &return_type,
			   std::shared_ptr<BlockNode> const &body, Types &&... args) : DefNode(parent, type, name), return_type(return_type), body(body), parameters(), block_id(-1), return_label_id(-1), local_var_total_size(-1)
	{
		int dummy[sizeof...(Types)] = {(parameters.push_back(args), 0)...};
	}

	int getBlockId() const;
	void setBlockId(int blockId);
	int getLocalVarTotalSize() const;
	void setLocalVarTotalSize(int localVarTotalSize);
	int getReturnLabelId() const;
	void setReturnLabelId(int returnLabelId);
};

class RefNode : public Node
{
private:
	std::string name;

public:
	RefNode();
	virtual ~RefNode() = default;
	RefNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name);
	const std::string &getName() const;
	void setName(const std::string &name);
};

class VarNode : public RefNode
{
private:
	std::shared_ptr<VarTypeNode> target_var;

public:
	VarNode();
	virtual ~VarNode() = default;
	VarNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name);

	const std::shared_ptr<VarTypeNode> &getTargetVar() const;
	void setTargetVar(const std::shared_ptr<VarTypeNode> &targetVar);
};

class FunNode : public RefNode
{
private:
	std::vector<std::shared_ptr<Node>> parameters;
	bool is_system_fun;
	std::shared_ptr<FunDefNode> target_fun;

public:
	FunNode();
	virtual ~FunNode() = default;
	std::vector<std::shared_ptr<Node>> &getParameters();
	void setParameters(const std::vector<std::shared_ptr<Node>> &parameters);
	void add_parameter(std::shared_ptr<Node> const &parameter);

	template <class... Types>
	FunNode(std::shared_ptr<Node> const &parent, ND_TYPE type, char const *const name, Types &&... args) : RefNode(parent, type, name), parameters(), is_system_fun(false), target_fun(nullptr)
	{
		int dummy[sizeof...(Types)] = {(parameters.push_back(args), 0)...};
	}

	bool isIsSystemFun() const;
	void setIsSystemFun(bool isSystemFun);
	const std::shared_ptr<FunDefNode> &getTargetFun() const;
	void setTargetFun(const std::shared_ptr<FunDefNode> &targetFun);
};

class StmtNode : public Node
{
private:
	std::shared_ptr<Node> stmt;

public:
	StmtNode();
	virtual ~StmtNode() = default;
	StmtNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &stmt);
	std::shared_ptr<Node> &getStmt();
	void setStmt(const std::shared_ptr<Node> &stmt);
};

class ParenthesisNode : public Node
{
private:
	std::shared_ptr<Node> node;

public:
	ParenthesisNode();
	virtual ~ParenthesisNode() = default;
	ParenthesisNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &node);
	std::shared_ptr<Node> &getNode();
	void setNode(const std::shared_ptr<Node> &node);
};

class LiteralNode : public Node
{
private:
	long long value;

public:
	LiteralNode();
	LiteralNode(std::shared_ptr<LiteralNode> const &literal_node);
	virtual ~LiteralNode() = default;
	LiteralNode(std::shared_ptr<Node> const &parent, ND_TYPE type, long long value);
	long long getValue() const;
	void setValue(long long value);
	bool setValue(char const *const value);
};

class ArrayNode : public Node
{
private:
	std::vector<std::shared_ptr<Node>> array;

public:
	ArrayNode();
	virtual ~ArrayNode() = default;
	std::vector<std::shared_ptr<Node>> &getArray();
	void setArray(const std::vector<std::shared_ptr<Node>> &array);
	void add_array(std::shared_ptr<Node> const &element);
	template <class... Types>
	ArrayNode(std::shared_ptr<Node> const &parent, ND_TYPE type, Types &&... args) : Node(parent, type), array()
	{
		int dummy[sizeof...(Types)] = {(array.push_back(args), 0)...};
	}
};

class ArrayAccessNode : public Node
{
private:
	std::vector<std::shared_ptr<Node>> indexes;
	std::shared_ptr<Node> target;

public:
	ArrayAccessNode();
	virtual ~ArrayAccessNode() = default;
	std::vector<std::shared_ptr<Node>> &getIndexes();
	void setIndexes(const std::vector<std::shared_ptr<Node>> &indexes);
	void add_index(std::shared_ptr<Node> const &index);
	const std::shared_ptr<Node> &getTarget() const;
	void setTarget(const std::shared_ptr<Node> &target);
	template <class... Types>
	ArrayAccessNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &target, Types &&... args) : Node(parent, type), target(target), indexes()
	{
		int dummy[sizeof...(Types)] = {(indexes.push_back(args), 0)...};
	}
};

class OperationNode : public Node
{
private:
	OP_TYPE operator_type;
	std::vector<std::shared_ptr<Node>> parameters;

public:
	OperationNode();
	virtual ~OperationNode() = default;
	OP_TYPE getOperatorType() const;
	void setOperatorType(OP_TYPE operatorType);
	std::vector<std::shared_ptr<Node>> &getParameters();
	void setParameters(const std::vector<std::shared_ptr<Node>> &parameters);
	void add_parameter(std::shared_ptr<Node> const &parameter);
	void reverse_parameters(void);
	template <class... Types>
	OperationNode(std::shared_ptr<Node> const &parent, ND_TYPE type, OP_TYPE operator_type, Types &&... args) : Node(parent, type), operator_type(operator_type), parameters()
	{
		int dummy[sizeof...(Types)] = {(parameters.push_back(args), 0)...};
	}
};

class IfElseNode : public Node
{
private:
	std::shared_ptr<Node> condition;
	std::shared_ptr<BlockNode> if_body;
	std::shared_ptr<BlockNode> else_body;

public:
	IfElseNode();
	virtual ~IfElseNode() = default;
	IfElseNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &condition, std::shared_ptr<BlockNode> const &if_body,
			   std::shared_ptr<BlockNode> const &else_body);
	const std::shared_ptr<Node> &getCondition() const;
	void setCondition(const std::shared_ptr<Node> &condition);
	const std::shared_ptr<BlockNode> &getElseBody() const;
	void setElseBody(const std::shared_ptr<BlockNode> &elseBody);
	const std::shared_ptr<BlockNode> &getIfBody() const;
	void setIfBody(const std::shared_ptr<BlockNode> &ifBody);
};

class WhileNode : public Node
{
private:
	std::shared_ptr<Node> condition;
	std::shared_ptr<BlockNode> body;
	int continue_label_id;
	int break_label_id;

public:
	WhileNode();
	virtual ~WhileNode() = default;
	WhileNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &condition, std::shared_ptr<BlockNode> const &body);
	const std::shared_ptr<BlockNode> &getBody() const;
	void setBody(const std::shared_ptr<BlockNode> &body);
	const std::shared_ptr<Node> &getCondition() const;
	void setCondition(const std::shared_ptr<Node> &condition);

	int getBreakLabelId() const;
	void setBreakLabelId(int breakLabelId);
	int getContinueLabelId() const;
	void setContinueLabelId(int continueLabelId);
};

class ContinueNode : public Node
{
private:
	std::shared_ptr<WhileNode> target;

public:
	ContinueNode();
	virtual ~ContinueNode() = default;
	ContinueNode(std::shared_ptr<Node> const &parent, ND_TYPE type);

	const std::shared_ptr<WhileNode> &getTarget() const;
	void setTarget(const std::shared_ptr<WhileNode> &target);
};

class BreakNode : public Node
{
private:
	std::shared_ptr<WhileNode> target;

public:
	BreakNode();
	virtual ~BreakNode() = default;
	BreakNode(std::shared_ptr<Node> const &parent, ND_TYPE type);

	const std::shared_ptr<WhileNode> &getTarget() const;
	void setTarget(const std::shared_ptr<WhileNode> &target);
};

class ReturnNode : public Node
{
private:
	std::shared_ptr<Node> return_value;
	std::shared_ptr<FunDefNode> target;

public:
	ReturnNode();
	virtual ~ReturnNode() = default;
	ReturnNode(std::shared_ptr<Node> const &parent, ND_TYPE type, std::shared_ptr<Node> const &return_value);
	const std::shared_ptr<Node> &getReturnValue() const;
	void setReturnValue(const std::shared_ptr<Node> &returnValue);

	const std::shared_ptr<FunDefNode> &getTarget() const;
	void setTarget(const std::shared_ptr<FunDefNode> &target);
};

class BlockNode : public Node
{
private:
	std::vector<std::shared_ptr<Node>> children;
	int id;

public:
	BlockNode();
	virtual ~BlockNode() = default;
	std::vector<std::shared_ptr<Node>> &getChildren();
	void setChildren(const std::vector<std::shared_ptr<Node>> &children);
	void add_child(std::shared_ptr<Node> const &child);
	template <class... Types>
	BlockNode(std::shared_ptr<Node> const &parent, ND_TYPE type, Types &&... args) : Node(parent, type), children(), id(-1)
	{
		int dummy[sizeof...(Types)] = {(children.push_back(args), 0)...};
	}

	int getId() const;
	void setId(int id);
};

#endif /* SRC_COMMON_NODE_H_ */
