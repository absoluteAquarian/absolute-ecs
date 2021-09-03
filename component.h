#ifndef ECS_COMPONENT_H_
#define ECS_COMPONENT_H_

#include <stdint.h>

#include "errorcode.h"

struct component;
struct component_table;
struct component_table_entry;

enum COMPONENT_TYPE{
	COMPONENT_STRING = 0,
	COMPONENT_INT32,
	
	/* How many component types exist */
	COMPONENT_COUNT
};

struct component{
	void *p_data;
	int32_t m_id;
};

struct component_table{
	struct component_table_entry **p_components;
};

struct component_table_entry{
	struct component *p_entry;
};

enum ERRCODE      C_init          (int32_t i_worldID, int32_t i_capacity);
void              C_ensureHasEntry(int32_t i_worldID, int32_t i_entityIdx, enum COMPONENT_TYPE e_type);
struct component *C_create        (int32_t i_worldID, int32_t i_entityIdx, void *data, enum COMPONENT_TYPE e_type);

#endif /* ECS_COMPONENT_H_ */