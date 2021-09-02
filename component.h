#ifndef COMPONENT_H_
#define COMPONENT_H_

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
	int32_t m_parent_tableidx;
};

struct component_table{
	struct component_table_entry **p_components;
};

struct component_table_entry{
	struct component *p_entry;
};

enum ERRCODE init_component_table(int32_t capacity);
void ensure_component_table_has_entry(int32_t entity_idx);
struct component *create_component(int32_t parent, void *data, enum COMPONENT_TYPE type);

extern struct component_table **g_componenttable;
extern int32_t g_componenttable_capacity;

#endif /* COMPONENT_H_ */