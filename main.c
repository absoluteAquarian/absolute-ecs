#include <stdio.h>
#include <stdint.h>

#include "errorcode.h"
#include "component.h"
#include "entity.h"
#include "utility.h"

int main(int argc, char** argv){
	puts("Running application...");
	
	int32_t obj_idx = create_entity();
	struct entity *ent = *(g_entitytable + obj_idx);
	
	puts("Created entity");
	
	/* TODO: creating components causes the app to hard crash with no error, nothing.  Investigate */
	
	char *name = "Hello, World!";
	ent->vtable->set_component(ent, create_component(obj_idx, name, COMPONENT_STRING));
	
	puts("Created component");
	
	struct component *data = ent->vtable->get_component(ent, COMPONENT_STRING);
	
	if(data){
		char* str = data->p_data;
		printf("component data: %s\n", str);
	}else
		puts("component was not registered\n");
	
	destroy_entity(ent);
	
	fflush(stdout);
	
	free(g_entitytable);
	free(g_componenttable);
	
	return 0;
}