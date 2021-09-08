#ifndef ECS_COMPONENT_H_
#define ECS_COMPONENT_H_ 1

#include <stdint.h>

#include "core.h"
#include "errorcode.h"

struct component;
struct component_table;
struct component_table_entry;

/* The base "types" of components.  Feel free to add additional values with other enums or just integers */
enum COMPONENT_TYPE{
	COMPONENT_STRING = 0,
	COMPONENT_INT32,
	COMPONENT_CHAR,
	COMPONENT_INT8,
	COMPONENT_INT16,
	COMPONENT_INT64,
	COMPONENT_UINT8,
	COMPONENT_UINT16,
	COMPONENT_UINT32,
	COMPONENT_UINT64,
	COMPONENT_SINGLE,
	COMPONENT_DOUBLE,
	COMPONENT_POINTER,
	
	DEFAULT_COMPONENT_COUNT
};

struct component{
	int32_t m_id;
	void *p_data;
};

struct component_table{
	struct component_table_entry **p_components;
};

struct component_table_entry{
	struct component *p_entry;
};

/* How many component types exist by default */
extern int32_t g_componentCount;

           enum ERRCODE      C_init             (int32_t i_worldIdx, int32_t i_capacity);
           void              C_ensureHasEntry   (int32_t i_worldIdx, int32_t i_entityIdx, int32_t e_type);
DLL_SYMBOL struct component *C_create           (int32_t i_worldIdx, int32_t i_entityIdx, void *data, int32_t e_type);
DLL_SYMBOL int32_t           C_getComponentCount();

#endif /* ECS_COMPONENT_H_ */