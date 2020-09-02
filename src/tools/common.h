/*
 * common.h
 *
 *  Created on: Jul 24, 2020
 *      Author: davies
 */

#ifndef SRC_TOOLS_COMMON_H_
#define SRC_TOOLS_COMMON_H_
#include <functional>

std::string& ReplaceStringInPlace(std::string &subject, const std::string &search, const std::string &replace);
bool read_file2string(char const *const file_path, std::string &str);
bool write_string2file(char const *const file_path, std::string const &str);
int expand_timing_function(std::string &source_file, char const *const macro_name, char const *const function_name);
bool read_dir(char const *const dir, std::ostringstream &oss);
bool fun_check(char const *const sysy_file_path, char const *const output_file_path);

#define OPERATE_MUL()				std::multiplies<long long>()
#define OPERATE_DIV()				std::divides<long long>()
#define OPERATE_MOD()				std::modulus<long long>()
#define OPERATE_ADD()				std::plus<long long>()
#define OPERATE_SUB()				std::minus<long long>()
#define OPERATE_GREATER()			std::greater<long long>()
#define OPERATE_LESS()				std::less<long long>()
#define OPERATE_GREATER_EQUAL()		std::greater_equal<long long>()
#define OPERATE_LESS_EQUAL()		std::less_equal<long long>()
#define OPERATE_EQUAL()				std::equal_to<long long>()
#define OPERATE_NOT_EQUAL()			std::not_equal_to<long long>()
#define OPERATE_LOGICAL_AND()		std::logical_and<long long>()
#define OPERATE_LOGICAL_OR()		std::logical_or<long long>()

template<typename OPERATE>
inline long long calculation(long long operand0, long long operand1, OPERATE operate) {
	return operate(operand0, operand1);
}

#endif /* SRC_TOOLS_COMMON_H_ */
