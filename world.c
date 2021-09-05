#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "component.h"
#include "core.h"
#include "entity.h"
#include "errorcode.h"
#include "utility.h"
#include "world.h"

struct world **g_worlds;
int32_t        g_worldCount;
struct world  *g_defaultWorld;

enum ERRCODE W_init(int32_t capacity){
	if(capacity <= 0)
		return RESULT_INIT_WORLD_TABLE_CAPACITY;
	
	if(!g_worlds){
		/* Create the world table */
		ALLOC(g_worlds, capacity);
		g_worldCount = capacity;
	}else if(capacity > g_worldCount){
		/* Resize the table */
		struct world **oldTable = g_worlds;
		ALLOC(g_worlds, capacity);
		
		/* Copy the old values to the new array */
		memcpy(g_worlds, oldTable, sizeof *g_worlds * g_worldCount);
		
		/* Free the old pointer */
		free_debug(oldTable, sizeof *g_worlds * g_worldCount);
		
		g_worldCount = capacity;
	}
	
	return RESULT_OK;
}

int32_t W_findFreeIndex(){
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	struct world **p_world = g_worlds;
	for(int w = 0; w < g_worldCount; w++, p_world++){
		if(*p_world && !(*p_world)->p_meta)
			log_err_lf(RESULT_NULL_METADATA);
		
		if(!*p_world || (*p_world)->p_meta->m_destroyed)
			return w;
	}
	
	return -1;
};

DLL_SYMBOL int32_t W_create(){
	static int nextID = 0;
	enum ERRCODE errcode;
	
#ifdef DEBUG_INFO
	printf("Creating world ID %d...\n", nextID);
#endif
	
	if(!g_worlds){
		errcode = W_init(1);
		log_err_lf(errcode);
	}
	
	int32_t freeIdx = W_findFreeIndex();
	if(freeIdx < 0){
		/* Allocate more space */
		freeIdx = g_worldCount;
		errcode = W_init(g_worldCount * 2);
		log_err_lf(errcode);
	}
	
	/* Create the world */
	struct world *obj = *(g_worlds + freeIdx);
	
	ENSURE_ALLOC(obj, 1);
	
#ifdef DEBUG_INFO
	puts("Initializing data members...");
#endif
	
	ENSURE_ALLOC(obj->p_meta, 1);
	
	obj->m_tableIdx = freeIdx;
	obj->m_etCapacity = 0;
	obj->m_ctCapacity = 0;
	obj->p_entityTable = NULL;
	obj->p_componentTable = NULL;
	
	obj->p_meta->m_id = nextID++;
	obj->p_meta->m_destroyed = false;
	
#ifdef DEBUG_INFO
	puts("Inserting world into worlds array...");
#endif
	
	/* Put the world in the table */
	*(g_worlds + freeIdx) = obj;
	
	g_defaultWorld = *g_worlds;
	
	return freeIdx;
};

DLL_SYMBOL enum ERRCODE W_destroy(struct world *p_world){
	if(!p_world)
		return RESULT_NULL_WORLD;
	
	if(!p_world->p_meta)
		return RESULT_NULL_METADATA;
	
	if(p_world->p_meta->m_destroyed)
		return RESULT_ENTITY_DESTROYED;
	
	p_world->p_meta->m_destroyed = true;
	p_world->p_meta->m_id = -1;
	p_world->m_tableIdx = -1;
	
	return RESULT_OK;
}

DLL_SYMBOL bool W_hasEntity(struct world *p_world, int32_t i_entityID, int32_t* p_entityIdx){
	if(!p_world)
		log_err_lf(RESULT_WORLD_UNINITIALIZED);
	
	struct entity **p_worldEntityTable = p_world->p_entityTable;
	if(!p_worldEntityTable)
		log_err_lf(RESULT_ENTITY_TABLE_UNINITIALIZED);
	
	for(int e = 0; e < p_world->m_etCapacity; e++, p_worldEntityTable++){
		if(p_worldEntityTable && !(*p_worldEntityTable)->p_meta->m_destroyed && (*p_worldEntityTable)->p_meta->m_id == i_entityID){
			*p_entityIdx = (*p_worldEntityTable)->m_tableIdx;
			return true;
		}
	}
	
	*p_entityIdx = -1;
	return false;
}

DLL_SYMBOL bool W_hasSystem(struct world *p_world, int32_t i_systemID, int32_t *p_systemIdx){
	if(!p_world)
		log_err_lf(RESULT_WORLD_UNINITIALIZED);
	
	struct system_base **p_worldSystemTable = p_world->p_systemTable;
	if(!p_worldSystemTable)
		log_err_lf(RESULT_SYSTEM_TABLE_UNINITIALIZED);
	
	for(int s = 0; s < p_world->m_stCapacity; s++, p_worldSystemTable++){
		if(p_worldSystemTable && !(*p_worldSystemTable)->p_meta)
			log_err_lf(RESULT_NULL_METADATA);
		
		if(p_worldSystemTable && !(*p_worldSystemTable)->p_meta->m_destroyed && (*p_worldSystemTable)->p_meta->m_id == i_systemID){
			*p_systemIdx = (*p_worldSystemTable)->m_tableIdx;
			return true;
		}
	}
	
	*p_systemIdx = -1;
	return false;
}

DLL_SYMBOL int32_t W_findEntityParentWorld(struct entity *p_entity){
	if(!p_entity)
		log_err_lf(RESULT_NULL_WORLD);
	
	if(!p_entity->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(p_entity->p_meta->m_destroyed)
		log_err_lf(RESULT_ENTITY_DESTROYED);
	
	struct world **p_worldTable = g_worlds;
	int idx = -1;
	for(int w = 0; w < g_worldCount; w++, p_worldTable++)
		if(p_worldTable && W_hasEntity(*p_worldTable, p_entity->p_meta->m_id, &idx))
			return w;

	return -1;
}