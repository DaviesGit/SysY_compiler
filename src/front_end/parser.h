/*
 * parser.h
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#ifndef SRC_FRONT_END_PARSER_H_
#define SRC_FRONT_END_PARSER_H_

#include <string>
#include <vector>
#include <memory>

#include "../common/node.h"
class Token;

extern char const *last_parse_error_msg;

std::shared_ptr<Node> gen_block_child_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end, bool &is_reach_end);
std::shared_ptr<BlockNode> gen_block_node(std::vector<std::shared_ptr<Token>>::const_iterator &current,
		std::vector<std::shared_ptr<Token>>::const_iterator &end);

std::shared_ptr<BlockNode> parse(std::vector<std::shared_ptr<Token>>const &tokens);
bool dump_node2json(std::shared_ptr<Node> const &node, std::ostringstream &oss);

#endif /* SRC_FRONT_END_PARSER_H_ */
