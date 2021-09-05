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

void absolute_ecs_init();
void absolute_ecs_run();
void create_entities();

int main(int argc, char** argv){
	AECS_Init();
	
	create_entities();
	
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
	int value = 26;
	E_setComponent(ent, C_create(g_defaultWorld->m_tableIdx, obj_idx, &value, COMPONENT_INT32));
}

#endif /* BUILD_DLL */