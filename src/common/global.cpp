/*
 * global.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: davies
 */

#include "global.h"

OPERATOR_INFO OPERATOR_INFOS[] = { { OP_UNARY_PLUS, 16, 1, AS_R2L }, //		/* +  */
		{ OP_UNARY_MINUS, 16, 1, AS_R2L }, //		/* -  */
		{ OP_LOGICAL_NOT, 16, 1, AS_R2L }, //		/* !  */
		{ OP_MUL, 15, 2, AS_L2R }, //				/* *  */
		{ OP_DIV, 15, 2, AS_L2R }, //				/* /  */
		{ OP_MOD, 15, 2, AS_L2R }, //				/* %  */
		{ OP_ADD, 14, 2, AS_L2R }, //				/* +  */
		{ OP_SUB, 14, 2, AS_L2R }, //				/* -  */
		{ OP_GREATER, 13, 2, AS_L2R }, //			/* >  */
		{ OP_LESS, 13, 2, AS_L2R }, //				/* <  */
		{ OP_GREATER_EQUAL, 13, 2, AS_L2R }, //		/* >= */
		{ OP_LESS_EQUAL, 13, 2, AS_L2R }, //			/* <= */
		{ OP_EQUAL, 12, 2, AS_L2R }, //				/* == */
		{ OP_NOT_EQUAL, 12, 2, AS_L2R }, //			/* != */
		{ OP_LOGICAL_AND, 11, 2, AS_L2R }, //		/* && */
		{ OP_LOGICAL_OR, 10, 2, AS_L2R }, //		/* || */
		{ OP_ASSIGNMENT, 9, 2, AS_R2L } //			/* =  */
};
const unsigned int OPERATOR_INFOS_LEN = sizeof(OPERATOR_INFOS)
		/ sizeof(OPERATOR_INFO);

std::map<OP_TYPE, OPERATOR_INFO> INIT_OPERATOR_INFO_MAP() {
	std::map<OP_TYPE, OPERATOR_INFO> map;
	for (int i = 0; i < OPERATOR_INFOS_LEN; ++i) {
		OPERATOR_INFO const &info = OPERATOR_INFOS[i];
		map[info.operator_type] = info;
	}
	return map;
}
std::map<OP_TYPE, OPERATOR_INFO> OPERATOR_INFO_MAP = INIT_OPERATOR_INFO_MAP();



//std::map<char const* const, OP_TYPE> INIT_OP_TYPE_MAP() {
//	std::map<char const* const, OP_TYPE> map;
//	return map;
//}
//std::map<char const* const, OP_TYPE> OP_TYPE_MAP = INIT_OP_TYPE_MAP();







long long next_global_id() {
	static long long id = 0;
	return ++id;
}











