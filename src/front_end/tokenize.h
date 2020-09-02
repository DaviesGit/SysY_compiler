/*
 * tokenize.h
 *
 *  Created on: Jul 28, 2020
 *      Author: davies
 */

#ifndef SRC_FRONT_END_TOKENIZE_H_
#define SRC_FRONT_END_TOKENIZE_H_

#include <string>
#include <vector>
#include <memory>

#include "../common/type.h"

extern char const *last_tokenize_error_msg;

class Token {
private:
	char const *start;
	int length;
	std::string name;
	TK_TYPE type;

public:
	Token();
	Token(char const *start, int length, char const *const name);
	Token(char const *start, int length, char const *const name, TK_TYPE type);
	~Token();

	int getLength() const;
	void setLength(int length);
	const std::string& getName() const;
	void setName(const std::string &name);
	const char* getStart() const;
	void setStart(const char *start);
	TK_TYPE getType() const;
	void setType(TK_TYPE type);

};

bool string2tokens(char const *const str, std::vector<std::shared_ptr<Token>> &tokens);
bool dump_tokens(std::vector<std::shared_ptr<Token>> const &tokens, std::string &str);

#endif /* SRC_FRONT_END_TOKENIZE_H_ */
