#include <stdio.h>
#include <stdint.h>

#include "errorcode.h"
#include "component.h"
#include "entity.h"
#include "utility.h"
#include "world.h"

int main(int argc, char** argv){
	/* Create the default world */
#ifdef DEBUG_INFO
	puts("Creating default world...");
#endif
	W_create();
	
#ifdef DEBUG_INFO
	puts("Creating entity...");
#endif
	int32_t obj_idx = E_create(g_defaultWorld->p_meta->m_id);
	struct entity *ent = *(g_defaultWorld->p_entityTable + obj_idx);
	
#ifdef DEBUG_INFO
	puts("Creating first component...");
#endif
	char *name = "Hello, World!";
	E_setComponent(ent, C_create(g_defaultWorld->p_meta->m_id, obj_idx, name, COMPONENT_STRING));
	
	struct component *data = E_getComponent(ent, COMPONENT_STRING);
	
	if(data){
		char* str = data->p_data;
		printf("component data: %s\n", str);
	}else
		puts(STRINGIFY(COMPONENT_STRING) " component was not registered");
	
#ifdef DEBUG_INFO
	puts("Creating second component...");
#endif
	int value = 26;
	E_setComponent(ent, C_create(g_defaultWorld->p_meta->m_id, obj_idx, &value, COMPONENT_INT32));
	
	data = E_getComponent(ent, COMPONENT_INT32);
	
	if(data){
		int* valuePtr = data->p_data;
		printf("component data: %d\n", *valuePtr);
	}else
		puts(STRINGIFY(COMPONENT_INT32) " component was not registered");
	
	E_destroy(ent);
	
	fflush(stdout);
	
	free(g_worlds);
	
	return 0;
}