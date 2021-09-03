#include <stdio.h>
#include <stdint.h>

#include "errorcode.h"
#include "component.h"
#include "entity.h"
#include "utility.h"

int main(int argc, char** argv){
	int32_t obj_idx = create_entity();
	struct entity *ent = *(g_entitytable + obj_idx);
	
	char *name = "Hello, World!";
	ent->vtable->set_component(ent, create_component(obj_idx, name, COMPONENT_STRING));
	
	struct component *data = ent->vtable->get_component(ent, COMPONENT_STRING);
	
	if(data){
		char* str = data->p_data;
		printf("component data: %s\n", str);
	}else
		puts(STRINGIFY(COMPONENT_STRING) " component was not registered");
	
	int value = 26;
	ent->vtable->set_component(ent, create_component(obj_idx, &value, COMPONENT_INT32));
	
	data = ent->vtable->get_component(ent, COMPONENT_INT32);
	
	if(data){
		int* valuePtr = data->p_data;
		printf("component data: %d\n", *valuePtr);
	}else
		puts(STRINGIFY(COMPONENT_INT32) " component was not registered");
	
	destroy_entity(ent);
	
	fflush(stdout);
	
	free(g_entitytable);
	free(g_componenttable);
	
	return 0;
}