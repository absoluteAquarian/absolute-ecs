#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "component.h"
#include "errorcode.h"
#include "entity.h"
#include "utility.h"

struct component_table **g_componenttable;
int32_t g_componenttable_capacity;

enum ERRCODE init_component_table(int32_t capacity){
	if(capacity <= 0)
		return RESULT_INIT_COMPONENT_TABLE_CAPACITY;
	
	if(!g_componenttable){
		/* Just create the table */
		printf("Size of requested table: %d\n", sizeof *g_componenttable * capacity);
		
		int perr = errno;
		errno = 0;
		g_componenttable = malloc_debug(sizeof *g_componenttable * capacity);
		
		if(perr != errno){
			fprintf(stderr, "(code %d) Error allocating memory: %s\n", errno, strerror(errno));
			fprintf(stderr, "Total allocation by \"malloc_debug\": %d\n", g_totalalloc);
		}
		
		g_componenttable_capacity = capacity;
		
		printf("(init_component_table, new) Initialized component table? %s\n", g_componenttable ? "true" : "false");
	}else if(capacity > g_componenttable_capacity){
		/* Resize the table */
		struct component_table **old_table = g_componenttable;
		g_componenttable = malloc_debug(sizeof *g_componenttable * capacity);
		
		/* Copy the old values to the new array */
		memcpy(g_componenttable, old_table, sizeof *g_componenttable * g_componenttable_capacity);
		
		/* Free the old pointer */
		free_debug(old_table, sizeof *g_componenttable * g_componenttable_capacity);
		
		g_componenttable_capacity = capacity;
		
		printf("(init_component_table, resize) Initialized component table? %s\n", g_componenttable ? "true" : "false");
	}
	
	return RESULT_OK;
}

void ensure_component_table_has_entry(int32_t entity_idx){
	if(!g_componenttable)
		log_err_lf(RESULT_COMPONENT_TABLE_UNINITIALIZED);
	
	/* Create a new entry for the entity if it doesn't exist already */
	if(!g_componenttable[entity_idx])
		g_componenttable[entity_idx] = malloc_debug(sizeof *g_componenttable);
	
	if(!g_componenttable[entity_idx]->p_components)
		g_componenttable[entity_idx]->p_components = malloc_debug(sizeof *g_componenttable[entity_idx]->p_components * COMPONENT_COUNT);
}

struct component *create_component(int32_t parent, void *data, enum COMPONENT_TYPE type){
	printf("Creating component of ID %d\n", (int32_t)type);
	
	enum ERRCODE errcode;
	
	struct entity *obj = *(g_entitytable + parent);
	
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(obj->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	if(type < 0 || type >= COMPONENT_COUNT)
		log_err_lf(RESULT_INVALID_COMPONENT_TYPE);
	
	puts("Type and parent are both valid");
	
	if(!g_componenttable){
		errcode = init_component_table(8);
		log_err_lf(errcode);
	}
	
	if(obj->m_tableidx >= g_componenttable_capacity){
		errcode = init_component_table(obj->m_tableidx);
		log_err_lf(errcode);
	}
	
	ensure_component_table_has_entry(obj->m_tableidx);
	
	/* Create the component */
	struct component *c_obj = malloc_debug(sizeof *c_obj);
	c_obj->p_data = data;
	c_obj->m_id = type;
	c_obj->m_parent_tableidx = obj->m_tableidx;
	
	return c_obj;
}