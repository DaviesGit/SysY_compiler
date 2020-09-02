/*
 * type.cpp
 *
 *  Created on: Jul 28, 2020
 *      Author: davies
 */

#include "type.h"

char const *const SY_KEYWORD[] = { "const", "int", "void", "if", "else", "while", "continue", "break", "return" };

char const *const SY_OPERATOR[] = { "+", "-", "!", "*", "/", "%", /*  "+", "-",  */
">", "<", ">=", "<=", "==", "!=", "&&", "||", "=" };

char const *const SY_OTHER_SYMBOL[] = { ";", "(", ")", ",", "{", "}", "[", "]" };

char const *const SY_MULTI_LETTER_SYMBOL[] = { ">=", "<=", "==", "!=", "&&", "||" };

char const *const SY_SINGLE_LETTER_SYMBOL[] = { "+", "-", "!", "*", "/", "%", ">", "<", "=", ";", "(", ")", ",", "{", "}", "[", "]" };

char const *const SY_SYSTEM_FUNCTION[] = { "getint", "getch", "getarray", "putint", "putch", "putarray", "putf", "starttime", "stoptime", "_sysy_starttime",
		"_sysy_stoptime", "malloc" };

const unsigned int SY_KEYWORD_LEN = sizeof(SY_KEYWORD) / sizeof(SY_KEYWORD[0]);
const unsigned int SY_OPERATOR_LEN = sizeof(SY_OPERATOR) / sizeof(SY_OPERATOR[0]);
const unsigned int SY_OTHER_SYMBOL_LEN = sizeof(SY_OTHER_SYMBOL) / sizeof(SY_OTHER_SYMBOL[0]);

const unsigned int SY_MULTI_LETTER_SYMBOL_LEN = sizeof(SY_MULTI_LETTER_SYMBOL) / sizeof(SY_MULTI_LETTER_SYMBOL[0]);

const unsigned int SY_SINGLE_LETTER_SYMBOL_LEN = sizeof(SY_SINGLE_LETTER_SYMBOL) / sizeof(SY_MULTI_LETTER_SYMBOL[0]);

const unsigned int SY_SYSTEM_FUNCTION_LEN = sizeof(SY_SYSTEM_FUNCTION) / sizeof(SY_SYSTEM_FUNCTION[0]);

char const *const SY_COMMENT[] = { SINGLE_LINE_COMMENT_START,
MULTI_LINE_COMMENT_START,
MULTI_LINE_COMMENT_END };
