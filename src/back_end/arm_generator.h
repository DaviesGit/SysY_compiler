/*
 * arm_generator.h
 *
 *  Created on: Aug 1, 2020
 *      Author: davies
 */

#ifndef SRC_BACK_END_ARM_GENERATOR_H_
#define SRC_BACK_END_ARM_GENERATOR_H_

#include <string>
#include <vector>
#include <memory>
#include "../common/node.h"

extern char const *last_arm_generate_error_msg;

bool gen_arm_assembly_code(std::shared_ptr<Node> const &node, std::ostringstream &oss);

#endif /* SRC_BACK_END_ARM_GENERATOR_H_ */
