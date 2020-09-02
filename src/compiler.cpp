#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "tools/common.h"
#include "front_end/tokenize.h"
#include "common/node.h"
#include "front_end/parser.h"
#include "front_end/semantic_analyzer.h"
#include "front_end/intermediate_representation.h"
#include "back_end/arm_generator.h"
#include "back_end/optimizer.h"

//https://blog.xojo.com/2017/12/06/compilers-101-overview-and-lexer/
//https://cs.lmu.edu/~ray/notes/ir/#:~:text=An%20intermediate%20representation%20is%20a,used%20in%20a%20retargetable%20compiler.
//https://azeria-labs.com/writing-arm-assembly-part-1/

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Argument count error!" << std::endl;
		return 1;
	}
	// const char *sysy_file_path = argv[1];
	const char *sysy_file_path = nullptr;
	const char *output_file_path = nullptr;
	int optimize = 0;
	// --argc;
	// ++argv;

	char option;
	// put ':' at the starting of the string so compiler can distinguish between '?' and ':'
	while ((option = getopt(argc, argv, ":So:O:")) != -1) { //get option from the getopt() method
		switch (option) {
		//For option i, r, l, print that these are options
		case 'S':
			// std::cout << "Given Option: " << option << std::endl;
			break;
		case 'O':
			optimize = std::stoi(optarg);
			// std::cout << "Given Option: O" << optimize << std::endl;
			break;
		case 'o': //here f is used for some file name
			// std::cout << "Ouput file: " << optarg << std::endl;
			output_file_path = optarg;
			break;
		case ':':
			std::cerr << "Option needs a value." << std::endl;
			return 2;
			break;
		case '?': //used for some unknown options
			std::cerr << "Unknown option: " << (char) optopt << std::endl;
			return 3;
			break;
			// default:
			//     std::cerr << "Unknown option: " << (char)optopt << std::endl;
		}
	}
	for (; optind < argc; optind++) { //when some extra arguments are passed
									  // std::cerr << "Given extra arguments: " << argv[optind] << std::endl;
									  // return 4;
		sysy_file_path = argv[optind];
	}
	set_optimize_level(optimize);

	//	optimize = std::string::npos != std::string(sysy_file_path).find("performance") || std::string::npos != std::string(sysy_file_path).find("bitset")
	//			|| std::string::npos != std::string(sysy_file_path).find("_mm");
	std::string sy_file;
	if (!read_file2string(sysy_file_path, sy_file))
		return 4;
	ReplaceStringInPlace(sy_file, "putf(", "printf(");
	ReplaceStringInPlace(sy_file, "_SYSY_N", "1024");

	expand_timing_function(sy_file, "starttime", "_sysy_starttime");
	expand_timing_function(sy_file, "stoptime", "_sysy_stoptime");


	std::vector<std::shared_ptr<Token>> tokens;
	if (!string2tokens(sy_file.c_str(), tokens)) {
		std::cerr << last_tokenize_error_msg << std::endl;
		return 5;
	}
	std::string str;
	if (!dump_tokens(tokens, str))
		return 6;
//	std::cout << str << std::endl;
	std::shared_ptr<BlockNode> root = parse(tokens);
	if (!root) {
		std::cerr << last_parse_error_msg << std::endl;
		return 7;
	}
//	return !root;

	if (!semantic_analyze(root)) {
		std::cerr << last_semantic_analyze_error_msg << std::endl;
		return 9;
	}
	std::ostringstream oss;
	if (!dump_node2json(std::dynamic_pointer_cast<Node>(root), oss))
		return 8;
//	std::cout << oss.str() << std::endl;

	oss.str("");
	if (!gen_arm_assembly_code(std::dynamic_pointer_cast<Node>(root), oss)) {
		std::cerr << last_arm_generate_error_msg << std::endl;
		return 9;
	}
	//	std::cout << oss.str() << std::endl;

	if (!write_string2file(output_file_path, oss.str()))
		return 10;
	std::cout << "All OK!" << std::endl;

	return 0;
}
