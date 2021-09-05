#ifndef ECS_SYSTEM_H_
#define ECS_SYSTEM_H_

#include <stdbool.h>
#include <stdint.h>

#include "errorcode.h"
#include "core.h"

struct system_base;
struct system_vtable;

/* The base member for any System.  Contains virtual methods and helpful fields */
struct system_base{
	/* The virtual functions */
	struct system_vtable *vtable;
	/* The next System to run.  If NULL, no System will run next and the update cycle will start over again */
	struct system_base   *p_next;
	/* Metadata shared by most structs in this project */
	struct ecs_metadata  *p_meta;
	/* Which types of components this system processes */
	int32_t              *p_validTypes;
	/* How many of said types there are */
	int32_t               m_validTypesCount;
	/* Where this system is located in a p_systemTable in a (struct world) */
	int32_t               m_tableIdx;
};

/* The virtual fuctions for a system */
struct system_vtable{
	/* The update function.  Parameters: system, world index, entity index */
	void (*update)     (struct system_base*, int32_t, int32_t);
	/* The validity function.  Parameters: system, world index, entity index */
	bool (*validEntity)(struct system_base*, int32_t, int32_t);
};

extern struct system_base *g_systemStart;

enum SYSTEM_TYPES{
	SYSTEM_PRINTING = 0
};

           enum ERRCODE S_init         (int32_t i_worldIdx, int32_t i_capacity);
           int32_t      S_findFreeIndex(int32_t i_worldIdx);
DLL_SYMBOL int32_t      S_create       (int32_t i_worldIdx, int32_t i_type, size_t s_allocSize);
           void         S_run          (int32_t i_systemIdx, int32_t i_worldIdx);
           bool         S_validEntity  (struct system_base *p_system, int32_t i_worldIdx, int32_t i_entityIdx);
DLL_SYMBOL void         S_insert       (struct system_base *p_system, int32_t i_typeToSortAfter);

#endif /* ECS_SYSTEM_H_ */