/*
 * type.h
 *
 *  Created on: Jul 28, 2020
 *      Author: davies
 */

#ifndef SRC_COMMON_TYPE_H_
#define SRC_COMMON_TYPE_H_

#define SINGLE_LINE_COMMENT_START "//"
#define MULTI_LINE_COMMENT_START "/*"
#define MULTI_LINE_COMMENT_END "*/"

enum VA_TYPE {
	VA_UNKNOWN, VA_INT, VA_VOID
};

enum TK_TYPE {
	TK_UNKNOWN,

	TK_OPERATOR, TK_SYMBOL, TK_DEC_DIGIT, TK_HEX_DIGIT, TK_OCT_DIGIT,

	TK_CONST, TK_INT, TK_VOID, TK_IF, TK_ELSE, TK_WHILE, TK_CONTINUE, TK_BREAK, TK_RETURN,

	TK_IDENTIFIER
};

enum ND_TYPE {
	ND_UNKNOWN,

	ND_TYPE_,
	ND_VAR_TYPE,
	ND_DEF,
	ND_VAR_DEF,
	ND_FUN_DEF,
	ND_REF,
	ND_VAR,
	ND_FUN,
	ND_STMT,
	ND_PARENTHESIS,
	ND_LITERAL,
	ND_ARRAY,
	ND_ARRAY_ACCESS,
	ND_OPERATION,
	ND_IF_ELSE,
	ND_WHILE,
	ND_CONTINUE,
	ND_BREAK,
	ND_RETURN,
	ND_BLOCK
};

enum OP_TYPE {
	OP_UNKNOWN, OP_NONE,

	/* +  */OP_UNARY_PLUS,
	/* -  */OP_UNARY_MINUS,
	/* !  */OP_LOGICAL_NOT,
	/* *  */OP_MUL,
	/* /  */OP_DIV,
	/* %  */OP_MOD,
	/* +  */OP_ADD,
	/* -  */OP_SUB,
	/* >  */OP_GREATER,
	/* <  */OP_LESS,
	/* >= */OP_GREATER_EQUAL,
	/* <= */OP_LESS_EQUAL,
	/* == */OP_EQUAL,
	/* != */OP_NOT_EQUAL,
	/* && */OP_LOGICAL_AND,
	/* || */OP_LOGICAL_OR,
	/* =  */OP_ASSIGNMENT,
};

extern char const *const SY_KEYWORD[];
extern char const *const SY_OPERATOR[];
extern char const *const SY_OTHER_SYMBOL[];
extern char const *const SY_MULTI_LETTER_SYMBOL[];
extern char const *const SY_SINGLE_LETTER_SYMBOL[];
extern char const *const SY_SYSTEM_FUNCTION[];
extern char const *const SY_COMMENT[];

extern const unsigned int SY_KEYWORD_LEN;
extern const unsigned int SY_OPERATOR_LEN;
extern const unsigned int SY_OTHER_SYMBOL_LEN;
extern const unsigned int SY_MULTI_LETTER_SYMBOL_LEN;
extern const unsigned int SY_SINGLE_LETTER_SYMBOL_LEN;
extern const unsigned int SY_SYSTEM_FUNCTION_LEN;

#endif /* SRC_COMMON_TYPE_H_ */
