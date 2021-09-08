#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "engine.h"
#include "entity.h"
#include "system.h"
#include "world.h"
#include "utility.h"

DLL_SYMBOL void AECS_Init(){
	/* Create the initial worlds */
#ifdef DEBUG_INFO
	puts("Creating initial worlds...");
#endif
	W_init(4);
	
	/* Create the default world, ID 0 */
	W_create();
	
	g_componentCount = -1;
}

DLL_SYMBOL void AECS_Run(){
	for(struct system_base *p_run = g_systemStart; p_run; p_run = p_run->p_next){
		S_readMessages(p_run);
		
		S_run(p_run->m_tableIdx, g_defaultWorld->m_tableIdx);
	}
}

DLL_SYMBOL void AECS_Deinit(){
	fflush(stdout);
	
	/* Free the data for all worlds */
	/* Ignore null entries for metadata and value members.  All that matters is that everything is freed properly. */
	for(int32_t i = 0; i < g_worldCount; i++){
		struct world *p_world = g_worlds[i];
		
		if(!p_world)
			continue;
		
		if(p_world->p_meta)
			free_debug(p_world->p_meta, sizeof *p_world->p_meta);
		
		/* Free the data for all components */
		for(int32_t c = 0; c < p_world->m_ctCapacity; c++){
			struct component_table *p_cTable = p_world->p_componentTable[c];
			
			if(!p_cTable)
				continue;
			
			for(int32_t cc = 0; cc < g_componentCount; cc++){
				struct component_table_entry *p_cTableEntry = p_cTable[cc];
				
				if(!p_cTableEntry || !p_cTableEntry->p_entry)
					continue;
				
				if(p_cTableEntry->p_entry->vtable){
					if(p_cTableEntry->p_entry->vtable->onClear)
						p_cTableEntry->p_entry->vtable->onClear(p_cTableEntry->p_entry);
					
					free_debug(p_cTableEntry->p_entry->vtable, sizeof *p_cTableEntry->p_entry->vtable);
				}
				
				free_debug(p_cTableEntry->p_entry, sizeof *p_cTableEntry->p_entry);
				free_debug(p_cTableEntry, sizeof *p_cTableEntry);
			}
			
			free_debug(p_cTable, sizeof *p_cTable);
		}
		
		free_debug(p_world->p_componentTable, sizeof *p_world->p_componentTable * p_world->m_ctCapacity);
		
		/* Free the data for all entities */
		for(int32_t e = 0; e < p_world->m_etCapacity; e++){
			struct entity *p_entity = p_world->p_entityTable[e];
			
			if(!p_entity)
				continue;
			
			if(p_entity->p_meta)
				free_debug(p_entity->p_meta, sizeof *p_entity->p_meta);
			
			free_debug(p_entity, sizeof *p_entity);
		}
		
		free_debug(p_world->p_entityTable, sizeof *p_world->p_entityTable * p_world->m_etCapacity);
		
		/* Free the data for all systems */
		for(int32_t s = 0; s < p_world->stCapacity; s++){
			struct system_base *p_system = p_world->p_systemTable[s];
			
			if(!p_system)
				continue;
			
			if(p_system->vtable)
				free_debug(p_system->vtable, sizeof *p_system->vtable);
			
			if(p_system->p_meta)
				free_debug(p_system->p_meta, sizeof *p_system->p_meta);
			
			for(int32_t m = 0; m < p_system->m_messageCount; m++){
				struct message *p_message = p_system->p_messages[m];
				
				if(!p_message)
					continue;
				
				if(p_message->vtable){
					if(p_message->vtable->onClear)
						p_message->vtable->onClear(p_message);
					
					free_debug(p_message->vtable, sizeof *p_message->vtable);
				}
				
				free_debug(p_message->p_data, sizeof *p_message->p_data);
				
				free_debug(p_message, sizeof *p_message);
			}
		}
		
		free_debug(p_world->p_systemTable, sizeof *p_world->p_systemTable * p_world->m_stCapacity);
		
		free_debug(p_world, sizeof *p_world);
	}
	
	free_debug(g_worlds, sizeof *g_worlds * g_worldCount);
}