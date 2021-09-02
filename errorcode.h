#ifndef ERRORCODE_H_
#define ERRORCODE_H_

#include <stdio.h>
#include <stdlib.h>

#define log_err_lf(errcode) log_err(errcode, __LINE__, __FILE__)

enum ERRCODE{
	RESULT_OK,
	RESULT_INIT_ENTITY_TABLE_CAPACITY,
	RESULT_INIT_COMPONENT_TABLE_CAPACITY,
	RESULT_NULL_ENTITY,
	RESULT_ENTITY_DESTROYED,
	RESULT_INVALID_COMPONENT_TYPE,
	RESULT_ENTITY_TABLE_UNINITIALIZED,
	RESULT_COMPONENT_TABLE_UNINITIALIZED
};

void log_err(enum ERRCODE errcode, int64_t line, const char* file);

#endif /* ERRORCODE_H_ */