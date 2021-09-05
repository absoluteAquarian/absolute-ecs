#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "component.h"
#include "core.h"
#include "errorcode.h"
#include "entity.h"
#include "utility.h"
#include "world.h"

int32_t g_componentCount;

enum ERRCODE C_init(int32_t i_worldIdx, int32_t i_capacity){
	if(i_capacity <= 0)
		return RESULT_INIT_COMPONENT_TABLE_CAPACITY;
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		return RESULT_WORLD_INVALID_INDEX;
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world)
		log_err_lf(RESULT_NULL_WORLD);
	
#ifdef DEBUG_INFO
	printf("Initializing/resizing component table for world %d...\n", p_world->p_meta->m_id);
	printf("Size of requested table: %zu\n", sizeof *p_world->p_componentTable * i_capacity);
#endif
	
	if(!p_world->p_componentTable || !p_world->m_ctCapacity){
#ifdef DEBUG_INFO
		puts("Initializing a new table...");
#endif
		
		/* Just create the table */
		int perr = errno;
		errno = 0;
		ALLOC(p_world->p_componentTable, i_capacity);
		
		if(perr != errno){
			fprintf(stderr, "(code %d) Error allocating memory: %s\n", errno, strerror(errno));
			fprintf(stderr, "Total allocation by \"malloc_debug\": %zu\n", g_totalalloc);
			log_err_lf(RESULT_INIT_COMPONENT_TABLE_CAPACITY);
		}
		
		p_world->m_ctCapacity = i_capacity;
		
#ifdef DEBUG_INFO
		printf("(C_init, new) Initialized component table? %s\n", p_world->p_componentTable ? "true" : "false");
#endif
	}else if(i_capacity > p_world->m_ctCapacity){
#ifdef DEBUG_INFO
		puts("Resizing an existing table...");
#endif

		/* Resize the table */
		struct component_table **old_table = p_world->p_componentTable;
		ALLOC(p_world->p_componentTable, i_capacity);
		
		/* Copy the old values to the new array */
		memcpy(p_world->p_componentTable, old_table, sizeof *p_world->p_componentTable * p_world->m_ctCapacity);
		
		/* Free the old pointer */
		free_debug(old_table, sizeof *p_world->p_componentTable * p_world->m_ctCapacity);
		
		p_world->m_ctCapacity = i_capacity;
		
#ifdef DEBUG_INFO
		printf("(C_init, resize) Initialized component table? %s\n", p_world->p_componentTable ? "true" : "false");
	}else
		printf("Table did not need to be initialized/resized (capacity: %d)\n", p_world->m_ctCapacity);
#else
	}
#endif
	
	return RESULT_OK;
}

void C_ensureHasEntry(int32_t i_worldIdx, int32_t i_entityIdx, int32_t e_type){
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		log_err_lf(RESULT_WORLD_INVALID_INDEX);
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world)
		log_err_lf(RESULT_NULL_WORLD);
	
	if(!p_world->p_componentTable)
		log_err_lf(RESULT_COMPONENT_TABLE_UNINITIALIZED);
	
	if(e_type >= C_getComponentCount()){
		int32_t oldCount = g_componentCount;
		g_componentCount = e_type + 1;
		
		/* Resize the component tables of all entities */
		for(int i = 0; i < p_world->m_etCapacity; i++){
			if(!p_world->p_entityTable[i] || !p_world->p_entityTable[i]->p_meta || p_world->p_entityTable[i]->p_meta->m_destroyed || !p_world->p_componentTable[i])
				continue;
			
			struct component_table_entry **table = p_world->p_componentTable[i]->p_components;
			ALLOC(p_world->p_componentTable[i]->p_components, C_getComponentCount());
			
			if(oldCount >= 0)
				memcpy(p_world->p_componentTable[i]->p_components, table, sizeof *p_world->p_componentTable[i]->p_components * oldCount);
			
			free_debug(table, sizeof *p_world->p_componentTable[i]->p_components * oldCount);
		}
	}
	
	/* Ensure the component table has enough room here */
	C_init(i_worldIdx, e_type);
	
	/* Create a new entry for the entity if it doesn't exist already */
	ENSURE_ALLOC(p_world->p_componentTable[i_entityIdx], 1);
	
	ENSURE_ALLOC(p_world->p_componentTable[i_entityIdx]->p_components, C_getComponentCount());
	
	for(int c = 0; c < C_getComponentCount(); c++)
		ENSURE_ALLOC(p_world->p_componentTable[i_entityIdx]->p_components[c], 1);
}

DLL_SYMBOL struct component *C_create(int32_t i_worldIdx, int32_t i_entityIdx, void *data, int32_t e_type){
	if(e_type < 0)
		log_err_lf(RESULT_INVALID_COMPONENT_TYPE);
	
#ifdef DEBUG_INFO
	printf("Creating component of ID %d\n", (int32_t)e_type);
#endif
	
	enum ERRCODE errcode;
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		log_err_lf(RESULT_WORLD_INVALID_INDEX);
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world)
		log_err_lf(RESULT_NULL_WORLD);
	
	struct entity *obj = *(p_world->p_entityTable + i_entityIdx);
	
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(!obj->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(obj->p_meta->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
#ifdef DEBUG_INFO
	puts("e_type and i_entityIdx are both valid");
#endif
	
	if(!p_world->p_componentTable || !p_world->m_ctCapacity){
		errcode = C_init(i_worldIdx, 8);
		log_err_lf(errcode);
	}
	
	if(obj->m_tableIdx >= p_world->m_ctCapacity){
		errcode = C_init(i_worldIdx, obj->m_tableIdx + 1);
		log_err_lf(errcode);
	}
	
	C_ensureHasEntry(i_worldIdx, obj->m_tableIdx, e_type);

#ifdef DEBUG_INFO
	puts("Entry slots have been created if needed");
#endif
	
	/* Create the component */
	struct component *c_obj = malloc_debug(sizeof *c_obj);
	c_obj->p_data = data;
	c_obj->m_id = e_type;
	
	return c_obj;
}

DLL_SYMBOL int32_t C_getComponentCount(){
	return g_componentCount;
}