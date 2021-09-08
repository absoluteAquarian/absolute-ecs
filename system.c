#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "errorcode.h"
#include "system.h"
#include "utility.h"
#include "world.h"

struct system_base *g_systemStart;

enum ERRCODE S_init(int32_t i_worldIdx, int32_t i_capacity){
	if(i_capacity <= 0)
		return RESULT_INIT_WORLD_TABLE_CAPACITY;
	
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		return RESULT_WORLD_INVALID_INDEX;
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world)
		return RESULT_NULL_WORLD;
	
#ifdef DEBUG_INFO
	printf("Initializing/resizing system table for world %d to capacity %d...\n", i_worldIdx, i_capacity);
#endif
	if(!p_world->p_systemTable || !p_world->m_stCapacity){
#ifdef DEBUG_INFO
		puts("Initializing table...");
#endif
		/* Just create the table */
		ALLOC(p_world->p_systemTable, i_capacity);
		p_world->m_stCapacity = i_capacity;
		
#ifdef DEBUG_INFO
		puts("Initialized table.");
#endif
	}else if(i_capacity > p_world->m_stCapacity){
#ifdef DEBUG_INFO
		puts("Resizing table...");
#endif
		/* Resize the table */
		struct system_base **old_table = p_world->p_systemTable;
		ALLOC(p_world->p_systemTable, i_capacity);
		
		/* Copy the old values to the new array */
		memcpy(p_world->p_systemTable, old_table, sizeof *p_world->p_systemTable * p_world->m_stCapacity);
		
		/* Free the old pointer */
		free_debug(old_table, sizeof *p_world->p_systemTable * p_world->m_etCapacity);
		
		p_world->m_stCapacity = i_capacity;
		
#ifdef DEBUG_INFO
		puts("Resized table.");
	}else
		printf("Table did not need to be initialized/resized (capacity: %d)\n", p_world->m_stCapacity);
#else
	}
#endif

	return RESULT_OK;
}

int32_t S_findFreeIndex(int32_t i_worldIdx){
	if(!g_worlds)
		log_err_lf(RESULT_WORLD_TABLE_UNINITIALIZED);
	
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		return RESULT_WORLD_INVALID_INDEX;
	
	struct world *p_world = g_worlds[i_worldIdx];
	
	if(!p_world->p_systemTable)
		log_err_lf(RESULT_SYSTEM_TABLE_UNINITIALIZED);
	
	struct system_base **table = p_world->p_systemTable;
	for(int i = 0; i < p_world->m_stCapacity; i++, table++){
		if(*table && !(*table)->p_meta)
			log_err_lf(RESULT_NULL_METADATA);
		
		if(!*table || (*table)->p_meta->m_destroyed)
			return i;
	}
	
	return -1;
}

DLL_SYMBOL int32_t S_create(int32_t i_worldIdx, int32_t i_type, size_t s_allocSize){
	enum ERRCODE errcode;
	
	if(i_type < 0)
		log_err_lf(RESULT_INVALID_COMPONENT_TYPE);
	
#ifdef DEBUG_INFO
	printf("Creating system ID %d...\n", i_type);
#endif

	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		log_err_lf(RESULT_WORLD_INVALID_INDEX);
	
	struct world *p_world = *(g_worlds + i_worldIdx);
	
	if(!p_world)
		log_err_lf(RESULT_NULL_WORLD);
	
	if(!p_world->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(!p_world->p_systemTable || !p_world->m_stCapacity){
		errcode = S_init(i_worldIdx, 8);
		log_err_lf(errcode);
	}
	
	int32_t free_idx = S_findFreeIndex(i_worldIdx);
	if(free_idx < 0){
		/* Allocate more space */
		free_idx = p_world->m_stCapacity;
		errcode = S_init(i_worldIdx, p_world->m_stCapacity + 1);
		log_err_lf(errcode);
	}
	
	struct system_base *obj = *(p_world->p_systemTable + free_idx);
	
	ENSURE_ALLOC_DIRECT(obj, s_allocSize);
	
	ENSURE_ALLOC(obj->p_meta, 1);
	
	ENSURE_ALLOC(obj->vtable, 1);
	
	ENSURE_ALLOC(obj->p_messages, 1);
	
	obj->m_tableIdx = free_idx;
	
	obj->p_meta->m_id = i_type;
	obj->p_meta->m_destroyed = false;
	
	*(p_world->p_systemTable + free_idx) = obj;
	
	return free_idx;
}

void S_run(int32_t i_systemIdx, int32_t i_worldIdx){
	if(i_worldIdx < 0 || i_worldIdx >= g_worldCount)
		log_err_lf(RESULT_WORLD_INVALID_INDEX);
	
	struct world *p_world = *(g_worlds + i_worldIdx);
	
	if(!p_world)
		log_err_lf(RESULT_NULL_WORLD);
	
	if(!p_world->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(!p_world->p_entityTable)
		log_err_lf(RESULT_ENTITY_TABLE_UNINITIALIZED);
	
	if(!p_world->p_componentTable)
		log_err_lf(RESULT_COMPONENT_TABLE_UNINITIALIZED);
	
	if(!p_world->p_systemTable)
		log_err_lf(RESULT_SYSTEM_TABLE_UNINITIALIZED);
	
	struct system_base *system = *(p_world->p_systemTable + i_systemIdx);
	
	if(!system)
		log_err_lf(RESULT_NULL_SYSTEM);
	
	if(!system->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(system->p_meta->m_destroyed)
		log_err_lf(RESULT_SYSTEM_DESTROYED);
	
	if(system->vtable->preUpdate)
		system->vtable->preUpdate(system, i_worldIdx);
	
	/* Parse over every entity and check if the system can apply to it.  If it can, do so */
	struct entity **p_worldEntityTable = p_world->p_entityTable;
	for(int e = 0; e < p_world->m_etCapacity; e++, p_worldEntityTable++){
		struct entity *p_entity = *p_worldEntityTable;
		/* Ignore NULL/destroyed entities, but not entities that have null metadata */
		if(!p_entity)
			continue;
		
		if(!p_entity->p_meta)
			log_err_lf(RESULT_NULL_METADATA);
		
		if(p_entity->p_meta->m_destroyed)
			continue;
		
		if(system->vtable->update && S_validEntity(system, i_worldIdx, p_entity->m_tableIdx))
			system->vtable->update(system, i_worldIdx, p_entity->m_tableIdx);
	}
	
	if(system->vtable->postUpdate)
		system->vtable->postUpdate(system, i_worldIdx);
}

bool S_validEntity(struct system_base *p_system, int32_t i_worldIdx, int32_t i_entityIdx){
	if(!p_system)
		log_err_lf(RESULT_NULL_SYSTEM);
	
	if(!p_system->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(p_system->p_meta->m_destroyed)
		log_err_lf(RESULT_SYSTEM_DESTROYED);
	
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
	
	int32_t numComponents = 0, matchedComponents = 0;
	int32_t *components = E_getComponents(obj, &numComponents);
	
	/* components will be NULL if the entity does not have any components */
	if(components && numComponents >= p_system->m_validTypesCount && p_system->m_validTypesCount > 0){
		for(int i = 0; i < numComponents; i++){
			struct component* p_component = E_getComponent(obj, components[i]);
			
			/* E_getComponent returns NULL if the entity doesn't have the component */
			if(!p_component)
				continue;
			
			for(int t = 0; t < p_system->m_validTypesCount; t++){
				int32_t type = p_system->p_validTypes[t];
				
				if(p_component->m_id == type){
					matchedComponents++;
					
					if(matchedComponents == p_system->m_validTypesCount){
						free_debug(components, sizeof(int32_t) * numComponents);
						return true;
					}
					
					break;
				}
			}
		}
	
		free_debug(components, sizeof(int32_t) * numComponents);
	}
	
	return false;
}

DLL_SYMBOL void S_insert(struct system_base *p_system, int32_t i_typeToSortAfter){
	if(!p_system)
		log_err_lf(RESULT_NULL_SYSTEM);
	
	if(!p_system->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(p_system->p_meta->m_destroyed)
		log_err_lf(RESULT_SYSTEM_DESTROYED);
	
	/* If the sorting list hasn't been declared yet, ignore i_typeToSortAfter */
	if(!g_systemStart){
		g_systemStart = p_system;
		return;
	}
	
	/* Ensure that no cyclical reference happens */
	struct system_base *p_listIter = g_systemStart;
	do{
		if(p_listIter == p_system)
			log_err_lf(RESULT_CYCLICAL_SYSTEM_STUCTURE);
		
		p_listIter = p_listIter->p_next;
	}while(p_listIter);
	
	/* Find the first system with the given type.  If none were found, print an error */
	p_listIter = g_systemStart;
	do{
		if(!p_listIter)
			log_err_lf(RESULT_NULL_SYSTEM);
		
		if(!p_listIter->p_meta)
			log_err_lf(RESULT_NULL_METADATA);
		
		if(p_listIter->p_meta->m_id == i_typeToSortAfter){
			struct system_base *p_next = p_listIter->p_next;
			p_listIter->p_next = p_system;
			p_system->p_next = p_next;
			return;
		}
		
		p_listIter = p_listIter->p_next;
	}while(p_listIter);
}

void S_readMessages(struct system_base *p_system){
	if(!p_system)
		log_err_lf(RESULT_NULL_SYSTEM);
	
	if(!p_system->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(p_system->p_meta->m_destroyed)
		log_err_lf(RESULT_SYSTEM_DESTROYED);
	
	if(!p_system->p_messages)
		log_err_lf(RESULT_NULL_SYSTEM_MESSAGES);
	
	if(p_system->vtable->readMessages)
		p_system->vtable->readMessages(p_system, W_findSystemParentWorld(p_system));
	
	if(!p_system->p_messages)
		log_err_lf(RESULT_NULL_SYSTEM_MESSAGES);
	
	free_debug(p_system->p_messages, sizeof *p_system->p_messages * p_system->m_messageCount);
	
	ALLOC(p_system->p_messages, 1);
	p_system->m_messageCount = 0;
}

DLL_SYMBOL void S_addMessage(struct system_base *p_system, struct message *p_message){
	if(!p_system)
		log_err_lf(RESULT_NULL_SYSTEM);
	
	if(!p_system->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(p_system->p_meta->m_destroyed)
		log_err_lf(RESULT_SYSTEM_DESTROYED);
	
	if(!p_system->p_messages)
		log_err_lf(RESULT_NULL_SYSTEM_MESSAGES);
	
	if(!p_message)
		log_err_lf(RESULT_NULL_MESSAGE);
	
	if(p_system->m_messageCount > 0){
		struct message **oldTable = p_system->p_messages;
		
		ALLOC(p_system->p_messages, p_system->m_messageCount + 1);
		
		memcpy(p_system->p_messages, oldTable, sizeof *p_system->p_messages * p_system->m_messageCount);
	}else
		*p_system->p_messages = p_message;
	
	p_system->m_messageCount++;
}