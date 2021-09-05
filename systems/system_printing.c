#include <stdint.h>

#include "../component.h"
#include "../errorcode.h"
#include "../system.h"
#include "../world.h"
#include "../utility.h"
#include "system_printing.h"

struct system_printing *S_createPrintingSystem(int32_t i_worldIdx, char* name){
	int32_t idx = S_create(i_worldIdx, SYSTEM_PRINTING, sizeof(struct system_printing));
	
	struct system_printing *ptr = (struct system_printing *)g_worlds[i_worldIdx]->p_systemTable[idx];
	ptr->super.vtable->update = &S_printingSystem_Update;
	ptr->super.m_validTypesCount = 2;
	ALLOC(ptr->super.p_validTypes, ptr->super.m_validTypesCount);
	ptr->super.p_validTypes[0] = COMPONENT_STRING;
	ptr->super.p_validTypes[1] = COMPONENT_INT32;
	ptr->p_name = name;
	
	return ptr;
}

void S_printingSystem_Update(struct system_base *p_system, int32_t i_worldIdx, int32_t i_entityIdx){
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
	
	struct system_printing *p_print = (struct system_printing*)p_system;
	
	printf("System \"%s\""
		"\n  World #%d"
		"\n  Entity #%d"
		"\n  " STRINGIFY(COMPONENT_STRING) ": %s"
		"\n  " STRINGIFY(COMPONENT_INT32) ": %d"
		"\n",
		p_print->p_name,
		i_worldIdx,
		i_entityIdx,
		(char*)E_getComponent(obj, COMPONENT_STRING)->p_data,
		*(int*)E_getComponent(obj, COMPONENT_INT32)->p_data);
}