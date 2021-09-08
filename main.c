#ifndef BUILD_DLL

#include <stdio.h>
#include <stdint.h>

#include "component.h"
#include "core.h"
#include "engine.h"
#include "entity.h"
#include "errorcode.h"
#include "system.h"
#include "utility.h"
#include "world.h"
#include "systems/system_printing.h"

void absolute_ecs_init();
void absolute_ecs_run();
void create_entities();
void create_systems();
void MAIN_systemSendMessages(struct system_base* p_system, int32_t i_worldIdx);
void MAIN_systemReadMessages(struct system_base* p_system, int32_t i_worldIdx);

int component_val;

int main(int argc, char** argv){
	AECS_Init();
	
	create_entities();
	create_systems();
	
	AECS_Run();
	
	/* Delete the first entity */
	E_destroy(*g_defaultWorld->p_entityTable);
	
	fflush(stdout);
	
	free_debug(g_worlds, sizeof *g_worlds * g_worldCount);
	
	return 0;
}

void create_entities(){
	
#ifdef DEBUG_INFO
	puts("Creating entity...");
#endif
	int32_t obj_idx = E_create(g_defaultWorld->p_meta->m_id);
	struct entity *ent = *(g_defaultWorld->p_entityTable + obj_idx);
	
#ifdef DEBUG_INFO
	puts("Creating first component...");
#endif
	char *name = "Hello, World!";
	E_setComponent(ent, C_create(g_defaultWorld->m_tableIdx, obj_idx, name, COMPONENT_STRING));
	
#ifdef DEBUG_INFO
	puts("Creating second component...");
#endif
	component_val = 26;
	E_setComponent(ent, C_create(g_defaultWorld->m_tableIdx, obj_idx, &component_val, COMPONENT_INT32));
}

void create_systems(){
	int32_t printSystem = S_createPrintingSystem(g_defaultWorld->m_tableIdx, "Jeff")->super.m_tableIdx;
	
	/* The type to sort after doesn't matter here since no systems have been added */
	S_insert(*(g_defaultWorld->p_systemTable + printSystem), 0);
	
	int32_t systemSend, systemRead;
	struct system_base *p_systemSend, *p_systemRead;
	
	systemSend = S_create(g_defaultWorld->m_tableIdx, SYSTEM_MESSAGE_SEND, sizeof(struct system_base));
	p_systemSend = *(g_defaultWorld->p_systemTable + systemSend);
	p_systemSend->vtable->postUpdate = &MAIN_systemSendMessages;
	
	S_insert(p_systemSend, SYSTEM_PRINTING);
	
	systemRead = S_create(g_defaultWorld->m_tableIdx, SYSTEM_MESSAGE_READ, sizeof(struct system_base));
	p_systemRead = *(g_defaultWorld->p_systemTable + systemRead);
	p_systemRead->vtable->readMessages = &MAIN_systemReadMessages;
	
	S_insert(p_systemRead, SYSTEM_MESSAGE_SEND);
}

void MAIN_systemSendMessages(struct system_base* p_system, int32_t i_worldIdx){
	if(!p_system)
		log_err_lf(RESULT_NULL_SYSTEM);
	
	if(!p_system->p_meta)
		log_err_lf(RESULT_NULL_METADATA);
	
	if(p_system->p_meta->m_destroyed)
		log_err_lf(RESULT_SYSTEM_DESTROYED);
	
	if(p_system->p_next){
		char* str = "Hello from the sending system!";
		
		S_addMessage(p_system->p_next, M_create(p_system->m_tableIdx, p_system->p_next->m_tableIdx, str, "STRING"));
	}
}

void MAIN_systemReadMessages(struct system_base* p_system, int32_t i_worldIdx){
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
	
	for(int i = 0; i < p_system->m_messageCount; i++){
		struct message *p_msg = p_system->p_messages[i];
		
		if(!p_msg || !p_msg->p_data)
			log_err_lf(RESULT_NULL_MESSAGE);
		
		if(p_msg->m_srcSystem < 0 || p_msg->m_destSystem >= p_world->m_stCapacity)
			log_err_lf(RESULT_INVALID_SYSTEM_INDEX);
		
		/* The only type of the messages sent is (char*), but caution should be used
		   when reading messages to account for the proper type.*/
		if(!strcmp(p_msg->p_data->p_typeName, "STRING")){
			char* p_msg_content = (char*)p_msg->p_data->p_content;
			
			printf("Message received by system #%d: %s\n", p_system->m_tableIdx, p_msg_content);
		}
	}
}

#endif /* BUILD_DLL */