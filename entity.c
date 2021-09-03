#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "component.h"
#include "errorcode.h"
#include "utility.h"

/* Defined here... */
struct entity* *g_entitytable;
int32_t g_entitytable_capacity;

enum ERRCODE init_entity_table(int32_t capacity){
	if(capacity <= 0)
		return RESULT_INIT_ENTITY_TABLE_CAPACITY;
	
	if(!g_entitytable){
		/* Just create the table */
		g_entitytable = malloc_debug(sizeof *g_entitytable * capacity);
		g_entitytable_capacity = capacity;
	}else if(capacity > g_entitytable_capacity){
		/* Resize the table */
		struct entity **old_table = g_entitytable;
		g_entitytable = malloc_debug(sizeof *g_entitytable * capacity);
		
		/* Copy the old values to the new array */
		memcpy(g_entitytable, old_table, sizeof *g_entitytable * g_entitytable_capacity);
		
		/* Free the old pointer */
		free_debug(old_table, sizeof *g_entitytable * g_entitytable_capacity);
		
		g_entitytable_capacity = capacity;
	}
	
	return RESULT_OK;
}

int32_t find_free_entity(){
	if(!g_entitytable)
		log_err_lf(RESULT_COMPONENT_TABLE_UNINITIALIZED);
	
	struct entity **table = g_entitytable;
	for(int i = 0; i < g_entitytable_capacity; i++, table++){
		if(!*table || (*table)->m_destroyed)
			return i;
	}
	
	return -1;
}

int32_t create_entity(){
	static int nextID = 0;
	enum ERRCODE errcode;
	
	if(!g_entitytable){
		errcode = init_entity_table(8);
		log_err_lf(errcode);
	}
	
	int32_t free_idx = find_free_entity();
	if(free_idx < 0){
		/* Allocate more space */
		free_idx = g_entitytable_capacity;
		errcode = init_entity_table(g_entitytable_capacity * 2);
		log_err_lf(errcode);
	}
	
	/* Create the entity */
	struct entity *existing = *(g_entitytable + free_idx);
	bool null_entity = !existing;
	
	struct entity *obj = null_entity ? malloc_debug(sizeof *obj) : existing;
	obj->m_id = nextID++;
	obj->m_tableidx = free_idx;
	obj->m_destroyed = false;
	if(null_entity)
		obj->vtable = malloc_debug(sizeof *obj->vtable);
	obj->vtable->has_component = &entity_has_component;
	obj->vtable->get_component = &entity_get_component;
	obj->vtable->set_component = &entity_set_component;
	
	/* Put the entity in the global table */
	*(g_entitytable + free_idx) = obj;
	
	return free_idx;
}

enum ERRCODE destroy_entity(struct entity *obj){
	if(!obj)
		return RESULT_NULL_ENTITY;
	
	if(obj->m_destroyed)
		return RESULT_ENTITY_DESTROYED;
	
	obj->m_destroyed = true;
	obj->m_id = -1;
	obj->m_tableidx = -1;
	
	return RESULT_OK;
}

bool entity_has_component(struct entity *obj, enum COMPONENT_TYPE type){
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(obj->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	return g_componenttable[obj->m_tableidx]->p_components[type]->p_entry != NULL;
}

struct component* entity_get_component(struct entity *obj, enum COMPONENT_TYPE type){
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(obj->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	if(!obj->vtable->has_component(obj, type))
		return NULL;
	
	return g_componenttable[obj->m_tableidx]->p_components[type]->p_entry;
}

void entity_set_component(struct entity *obj, struct component *data){
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(obj->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	ensure_component_table_has_entry(obj->m_tableidx, data->m_id);
	
	g_componenttable[obj->m_tableidx]->p_components[data->m_id]->p_entry = data;
}