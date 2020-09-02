/*
 * optimizer.h
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#ifndef SRC_BACK_END_OPTIMIZER_H_
#define SRC_BACK_END_OPTIMIZER_H_

#include <string>
#include <vector>
#include <memory>

#include "../common/node.h"
#include "../tools/common.h"
class Token;

extern char const *last_optimize_error_msg;

bool evaluate_literal_node(std::shared_ptr<Node> const &node, std::shared_ptr<LiteralNode> &literal);

int get_optimize_level();
void set_optimize_level(int _optimize);
bool merge_block_function_call(std::shared_ptr<BlockNode> &block);

#endif /* SRC_BACK_END_OPTIMIZER_H_ */
