/*
 * global.h
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#ifndef SRC_COMMON_GLOBAL_H_
#define SRC_COMMON_GLOBAL_H_

#include <map>

#include "type.h"

enum SY_ASSOCIATIVITY {
	AS_L2R, AS_R2L
};
typedef struct OPERATOR_INFO {
	OP_TYPE operator_type;
	int precedence;
	int parameter_number;
	SY_ASSOCIATIVITY associativity;
} OPERATOR_INFO;

extern std::map<OP_TYPE, OPERATOR_INFO> OPERATOR_INFO_MAP;

long long next_global_id();

#endif /* SRC_COMMON_GLOBAL_H_ */
