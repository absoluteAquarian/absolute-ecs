#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "component.h"
#include "core.h"
#include "errorcode.h"
#include "utility.h"
#include "world.h"

enum ERRCODE E_init(int32_t i_worldIdx, int32_t capacity){
	if(capacity <= 0)
		return RESULT_INIT_ENTITY_TABLE_CAPACITY;
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		return RESULT_WORLD_INVALID_INDEX;
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world)
		return RESULT_NULL_WORLD;
	
#ifdef DEBUG_INFO
	printf("Initializing/resizing entity table for world %d to capacity %d...\n", i_worldIdx, capacity);
#endif
	
	if(!p_world->p_entityTable || !p_world->m_etCapacity){
#ifdef DEBUG_INFO
		puts("Initializing table...");
#endif
		/* Just create the table */
		ALLOC(p_world->p_entityTable, capacity);
		p_world->m_etCapacity = capacity;
		
#ifdef DEBUG_INFO
		puts("Initialized table.");
#endif
	}else if(capacity > p_world->m_etCapacity){
#ifdef DEBUG_INFO
		puts("Resizing table...");
#endif
		/* Resize the table */
		struct entity **old_table = p_world->p_entityTable;
		ALLOC(p_world->p_entityTable, capacity);
		
		/* Copy the old values to the new array */
		memcpy(p_world->p_entityTable, old_table, sizeof *p_world->p_entityTable * p_world->m_etCapacity);
		
		/* Free the old pointer */
		free_debug(old_table, sizeof *p_world->p_entityTable * p_world->m_etCapacity);
		
		p_world->m_etCapacity = capacity;
		
#ifdef DEBUG_INFO
		puts("Resized table.");
	}else
		printf("Table did not need to be initialized/resized (capacity: %d)\n", p_world->m_etCapacity);
#else
	}
#endif
	
	return RESULT_OK;
}

int32_t E_findFreeIndex(int32_t i_worldIdx){
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		return RESULT_WORLD_INVALID_INDEX;
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world->p_entityTable)
		log_err_lf(RESULT_ENTITY_TABLE_UNINITIALIZED);
	
	struct entity **table = p_world->p_entityTable;
	for(int i = 0; i < p_world->m_etCapacity; i++, table++){
		if(*table && !(*table)->p_meta)
			log_err_lf(RESULT_NULL_METADATA);
		
		if(!*table || (*table)->p_meta->m_destroyed)
			return i;
	}
	
	return -1;
}

DLL_SYMBOL int32_t E_create(int32_t i_worldIdx){
	static int nextID = 0;
	enum ERRCODE errcode;
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		log_err_lf(RESULT_WORLD_INVALID_INDEX);
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world)
		log_err_lf(RESULT_NULL_WORLD);
	
#ifdef DEBUG_INFO
	printf("Ensuring entity table for world %d is initialized...\n", i_worldIdx);
#endif
	
	if(!p_world->p_entityTable || !p_world->m_etCapacity){
		errcode = E_init(i_worldIdx, 8);
		log_err_lf(errcode);
	}
	
#ifdef DEBUG_INFO
	puts("Finding free index to use for this entity...");
#endif
	
	int32_t free_idx = E_findFreeIndex(i_worldIdx);
	if(free_idx < 0){
		/* Allocate more space */
		free_idx = p_world->m_etCapacity;
		errcode = E_init(i_worldIdx, p_world->m_etCapacity + 1);
		log_err_lf(errcode);
	}
#ifdef DEBUG_INFO
	printf("Index to insert entity at: %d\n", free_idx);
#endif
	
	/* Create the entity */
	struct entity *obj = *(p_world->p_entityTable + free_idx);
	
	ENSURE_ALLOC(obj, 1);
	
#ifdef DEBUG_INFO
	puts("Initializing data members...");
#endif
	
	ENSURE_ALLOC(obj->p_meta, 1);
	
	obj->m_tableIdx = free_idx;
	
	obj->p_meta->m_id = nextID++;
	obj->p_meta->m_destroyed = false;
	
#ifdef DEBUG_INFO
	puts("Inserting entity into entity table...");
#endif
	
	/* Put the entity in the table */
	*(p_world->p_entityTable + free_idx) = obj;
	
	return free_idx;
}

DLL_SYMBOL enum ERRCODE E_destroy(struct entity *obj){
	if(!obj)
		return RESULT_NULL_ENTITY;
	
	if(!obj->p_meta)
		return RESULT_NULL_METADATA;
	
	if(obj->p_meta->m_destroyed)
		return RESULT_ENTITY_DESTROYED;
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	obj->p_meta->m_destroyed = true;
	obj->p_meta->m_id = -1;
	obj->m_tableIdx = -1;
	
	return RESULT_OK;
}

DLL_SYMBOL bool E_hasComponent(struct entity *obj, int32_t type){
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(!obj->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(obj->p_meta->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(type < 0)
		log_err_lf(RESULT_INVALID_COMPONENT_TYPE);
	
	int32_t worldIdx = W_findEntityParentWorld(obj);
	if(worldIdx < 0)
		log_err_lf(RESULT_ENTITY_NOT_BOUND_TO_WORLD);
	
	if(!g_worlds[worldIdx])
		log_err_lf(RESULT_NULL_WORLD);
	
	C_ensureHasEntry(worldIdx, obj->m_tableIdx, type);
	
	if(!g_worlds[worldIdx]->p_componentTable || !g_worlds[worldIdx]->p_componentTable[obj->m_tableIdx])
		log_err_lf(RESULT_COMPONENT_TABLE_UNINITIALIZED);
	
	return g_worlds[worldIdx]->p_componentTable[obj->m_tableIdx]->p_components[type]->p_entry != NULL;
}

DLL_SYMBOL struct component* E_getComponent(struct entity *obj, int32_t type){
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(!obj->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(obj->p_meta->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(type < 0)
		log_err_lf(RESULT_INVALID_COMPONENT_TYPE);
	
	int32_t worldIdx = W_findEntityParentWorld(obj);
	if(worldIdx < 0)
		log_err_lf(RESULT_ENTITY_NOT_BOUND_TO_WORLD);
	
	C_ensureHasEntry(worldIdx, obj->m_tableIdx, type);
	
	if(!E_hasComponent(obj, type))
		return NULL;
	
	return g_worlds[worldIdx]->p_componentTable[obj->m_tableIdx]->p_components[type]->p_entry;
}

DLL_SYMBOL int32_t *E_getComponents(struct entity *obj, int32_t* numComponents){
	*numComponents = 0;
	
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(!obj->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(obj->p_meta->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	int32_t worldIdx = W_findEntityParentWorld(obj);
	if(worldIdx < 0)
		log_err_lf(RESULT_ENTITY_NOT_BOUND_TO_WORLD);
	
	struct world *p_world = g_worlds[worldIdx];
	
	int nextIdx = 0;
	int32_t* arr = malloc_debug(sizeof(int32_t) * p_world->m_ctCapacity);
	for(int i = 0; i < p_world->m_ctCapacity; i++){
		C_ensureHasEntry(worldIdx, obj->m_tableIdx, i);
		
		if(E_hasComponent(obj, i)){
			*(arr + nextIdx) = i;
			nextIdx++;
		}
	}
	
	if(!nextIdx)
		return NULL;
	
	int32_t *ret = malloc_debug(sizeof(int32_t) * nextIdx);
	memcpy(ret, arr, sizeof(int32_t) * nextIdx);
	
	free_debug(arr, sizeof(int) * p_world->m_ctCapacity);
	
	*numComponents = nextIdx;
	
	return ret;
}

DLL_SYMBOL void E_setComponent(struct entity *obj, struct component *data){
	if(!obj)
		log_err_lf(RESULT_NULL_ENTITY);
	
	if(!obj->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(obj->p_meta->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	int32_t worldIdx = W_findEntityParentWorld(obj);
	if(worldIdx < 0)
		log_err_lf(RESULT_ENTITY_NOT_BOUND_TO_WORLD);
	
	C_ensureHasEntry(worldIdx, obj->m_tableIdx, data->m_id);
	
	g_worlds[worldIdx]->p_componentTable[obj->m_tableIdx]->p_components[data->m_id]->p_entry = data;
}