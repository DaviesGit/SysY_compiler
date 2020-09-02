/*
 * common.cpp
 *
 *  Created on: Jul 24, 2020
 *      Author: davies
 */

#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <dirent.h>

#include "common.h"

std::string& ReplaceStringInPlace(std::string &subject, const std::string &search, const std::string &replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

bool read_file2string(char const *const file_path, std::string &str) {
	std::ifstream file(file_path);
	if (!file)
		return false;
	str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return true;
}

bool write_string2file(char const *const file_path, std::string const &str) {
	std::ofstream file(file_path);
	if (!file)
		return false;
	file << str;
	return true;
}

int expand_timing_function(std::string &source_file, char const *const macro_name, char const *const function_name) {
	int count = 0;
	int current_line = 0;
	std::ostringstream oss;
	std::size_t last_position = 0;
	std::size_t current_position = 0;
	char const *const _source_file = source_file.c_str();
	while (std::string::npos != (current_position = source_file.find(macro_name, last_position))) {
//		if (*(_source_file + current_position + strlen(macro_name) + 1 )!= ')')
//			continue;
		++count;
		oss.write(_source_file + last_position, current_position - last_position);
		for (int i = last_position; i < current_position; ++i) {
			if ('\n' == _source_file[i])
				++current_line;
		}
		oss << function_name << "(" << current_line + 1 << ")";
		last_position = current_position + strlen(macro_name) + 2;
	}
	oss.write(_source_file + last_position, strlen(_source_file) - last_position);
	source_file = oss.str();
	return count;
}

