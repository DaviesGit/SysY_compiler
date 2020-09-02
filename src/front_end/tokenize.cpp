/*
 * tokenize.cpp
 *
 *  Created on: Jul 28, 2020
 *      Author: davies
 */

#include <string.h>
#include <sstream>

#include "tokenize.h"

char const *last_tokenize_error_msg = nullptr;

Token::Token() :
		Token(nullptr, -1, nullptr) {

}
Token::Token(char const *start, int length, char const *const name) :
		Token(start, length, name, TK_UNKNOWN) {

}
Token::Token(char const *start, int length, char const *const name, TK_TYPE type) :
		start(start), length(length), name(name), type(type) {

}
Token::~Token() {

}

int Token::getLength() const {
	return length;
}

void Token::setLength(int length) {
	this->length = length;
}

const std::string& Token::getName() const {
	return name;
}

void Token::setName(const std::string &name) {
	this->name = name;
}

const char* Token::getStart() const {
	return start;
}

void Token::setStart(const char *start) {
	this->start = start;
}

TK_TYPE Token::getType() const {
	return type;
}

void Token::setType(TK_TYPE type) {
	this->type = type;
}

//////////////////

inline bool is_identifier_start(char const *const ptr) {
	return isalpha(*ptr) || *ptr == '_';
}

inline bool is_start_with(char const *const s1, char const *const s2) {
	return !strncmp(s1, s2, strlen(s2));
}

inline bool is_identifier_start_letter(char ch) {
	return isalpha(ch) || ch == '_';
}

inline bool is_identifier_letter(char ch) {
	return isalpha(ch) || isdigit(ch) || ch == '_';
}

inline std::shared_ptr<Token> new_identifier_token(char const *&current) {
	char const *start = current;
	while (is_identifier_letter(*++current))
		;
	char const *end = current;
	return std::shared_ptr<Token>(new Token(start, end - start, std::string(start, end).c_str()));
}

inline std::shared_ptr<Token> new_literal_token(char const *&current) {
	char const *start = current;
	if ('0' != *current) {
		while (isdigit(*++current))
			;
		char const *end = current;
		return std::shared_ptr<Token>(new Token(start, end - start, std::string(start, end).c_str()));
	}
	++current;
	if ('x' != *current && 'X' != *current) {
		if (strchr("01234567", *current))
			while (strchr("01234567", *++current))
				;
		char const *end = current;
		return std::shared_ptr<Token>(new Token(start, end - start, std::string(start, end).c_str()));
	}
	{
		while (strchr("0123456789abcdefABCDEF", *++current))
			;
		char const *end = current;
		return std::shared_ptr<Token>(new Token(start, end - start, std::string(start, end).c_str()));
	}
}

inline bool is_multi_letter_symbol(char const *current) {
	for (int i = 0; i < SY_MULTI_LETTER_SYMBOL_LEN; ++i) {
		if (is_start_with(current, SY_MULTI_LETTER_SYMBOL[i]))
			return true;
	}
	return false;
}

inline std::shared_ptr<Token> new_multi_letter_symbol(char const *&current) {
	char const *start = current;
	for (int i = 0; i < SY_MULTI_LETTER_SYMBOL_LEN; ++i) {
		if (is_start_with(current, SY_MULTI_LETTER_SYMBOL[i])) {
			current += strlen(SY_MULTI_LETTER_SYMBOL[i]);
			char const *end = current;
			return std::shared_ptr<Token>(new Token(start, end - start, std::string(start, end).c_str()));
		}
	}
	//never reached!
	return nullptr;
}

inline bool is_single_letter_symbol(char const *current) {
	for (int i = 0; i < SY_SINGLE_LETTER_SYMBOL_LEN; ++i) {
		if (is_start_with(current, SY_SINGLE_LETTER_SYMBOL[i]))
			return true;
	}
	return false;
}

inline std::shared_ptr<Token> new_single_letter_symbol(char const *&current) {
	char const *start = current;
	for (int i = 0; i < SY_SINGLE_LETTER_SYMBOL_LEN; ++i) {
		if (is_start_with(current, SY_SINGLE_LETTER_SYMBOL[i])) {
			current += strlen(SY_SINGLE_LETTER_SYMBOL[i]);
			char const *end = current;
			return std::shared_ptr<Token>(new Token(start, end - start, std::string(start, end).c_str()));
		}
	}
	//never reached!
	return nullptr;
}

bool is_include_in_array(char const *const*const array, const unsigned int array_len, char const *const str) {
	for (int i = 0; i < array_len; ++i) {
		if (!strcmp(str, array[i])) {
			return true;
		}
	}
	return false;
}

bool set_token_type(std::vector<std::shared_ptr<Token>> &tokens) {
	for (std::vector<std::shared_ptr<Token>>::iterator it = tokens.begin(), end = tokens.end(); it != end; ++it) {
		char const *const name = (*it)->getName().c_str();

		if (is_include_in_array(SY_OPERATOR, SY_OPERATOR_LEN, name)) {
			(*it)->setType(TK_OPERATOR);
			continue;
		}
		if (is_include_in_array(SY_OTHER_SYMBOL, SY_OTHER_SYMBOL_LEN, name)) {
			(*it)->setType(TK_SYMBOL);
			continue;
		}
		if (isdigit(*name)) {
			if ('0' != *name)
				(*it)->setType(TK_DEC_DIGIT);
			else if ('x' == *(name + 1) || 'X' == *(name + 1))
				(*it)->setType(TK_HEX_DIGIT);
			else
				(*it)->setType(TK_OCT_DIGIT);
			continue;
		}
		TK_TYPE type;
		if (!strcmp("const", name))
			type = TK_CONST;
		else if (!strcmp("int", name))
			type = TK_INT;
		else if (!strcmp("void", name))
			type = TK_VOID;
		else if (!strcmp("if", name))
			type = TK_IF;
		else if (!strcmp("else", name))
			type = TK_ELSE;
		else if (!strcmp("while", name))
			type = TK_WHILE;
		else if (!strcmp("continue", name))
			type = TK_CONTINUE;
		else if (!strcmp("break", name))
			type = TK_BREAK;
		else if (!strcmp("return", name))
			type = TK_RETURN;
		else
			type = TK_IDENTIFIER;
		(*it)->setType(type);
		continue;
		return false;
	}
	return true;
}

bool string2tokens(char const *const str, std::vector<std::shared_ptr<Token>> &tokens) {

	char const *current = str;

	while (*current) {
		//skip space \s\r\n...
		if (isspace(*current)) {
			++current;
			continue;
		}
		//skip SINGLE_LINE_COMMENT
		if (is_start_with(current, SINGLE_LINE_COMMENT_START)) {
			current += 2;
			while ('\n' != *(current++))
				;
			continue;
		}
		//skip MULTI_LINE_COMMENT
		if (is_start_with(current, MULTI_LINE_COMMENT_START)) {
			while (!is_start_with(++current, MULTI_LINE_COMMENT_END))
				;
			current += 2;
			continue;
		}
		//identifier
		if (is_identifier_start_letter(*current)) {
			tokens.push_back(new_identifier_token(current));
			continue;
		}
		//literal digit
		if (isdigit(*current)) {
			tokens.push_back(new_literal_token(current));
			continue;
		}
		//multi_letter_symbol
		if (is_multi_letter_symbol(current)) {
			tokens.push_back(new_multi_letter_symbol(current));
			continue;
		}
		//single_letter_symbol
		if (is_single_letter_symbol(current)) {
			tokens.push_back(new_single_letter_symbol(current));
			continue;
		}

		last_tokenize_error_msg = "Unexpected token!";
		//never reached!
		return false;
	}


	return set_token_type(tokens);
}

bool dump_tokens(std::vector<std::shared_ptr<Token>> const &tokens, std::string &str) {
	std::ostringstream str_stream;

	for (std::vector<std::shared_ptr<Token>>::const_iterator it = tokens.begin(), end = tokens.end(); it != end; ++it) {
		char const *type;
		switch ((*it)->getType()) {
		case TK_UNKNOWN:
			type = "TK_UNKNOWN";
			break;
		case TK_OPERATOR:
			type = "TK_OPERATOR";
			break;
		case TK_SYMBOL:
			type = "TK_SYMBOL";
			break;
		case TK_DEC_DIGIT:
			type = "TK_DEC_DIGIT";
			break;
		case TK_HEX_DIGIT:
			type = "TK_HEX_DIGIT";
			break;
		case TK_OCT_DIGIT:
			type = "TK_OCT_DIGIT";
			break;
		case TK_CONST:
			type = "TK_CONST";
			break;
		case TK_INT:
			type = "TK_INT";
			break;
		case TK_VOID:
			type = "TK_VOID";
			break;
		case TK_IF:
			type = "TK_IF";
			break;
		case TK_ELSE:
			type = "TK_ELSE";
			break;
		case TK_WHILE:
			type = "TK_WHILE";
			break;
		case TK_CONTINUE:
			type = "TK_CONTINUE";
			break;
		case TK_BREAK:
			type = "TK_BREAK";
			break;
		case TK_RETURN:
			type = "TK_RETURN";
			break;
		case TK_IDENTIFIER:
			type = "TK_IDENTIFIER";
			break;
		default:
			type = "This is an error!!!!";
		}
		str_stream << (*it)->getName() << "/*" << type << "*/" << ' ';
	}
	str = str_stream.str();
	return true;
}

