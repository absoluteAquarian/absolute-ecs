#ifndef ECS_ENTITY_H_
#define ECS_ENTITY_H_

#include <stdbool.h>
#include <stdint.h>

#include "component.h"
#include "core.h"
#include "errorcode.h"

struct entity;

struct entity{
	struct ecs_metadata *p_meta;
	
	int32_t m_tableIdx;
};

           enum ERRCODE      E_init         (int32_t i_worldIdx, int32_t capacity);
           int32_t           E_findFreeIndex(int32_t i_worldIdx);
DLL_SYMBOL int32_t           E_create       (int32_t i_worldIdx);
DLL_SYMBOL enum ERRCODE      E_destroy      (struct entity *obj);
DLL_SYMBOL bool              E_hasComponent (struct entity *obj, int32_t type);
DLL_SYMBOL struct component *E_getComponent (struct entity *obj, int32_t type);
DLL_SYMBOL int32_t          *E_getComponents(struct entity *obj, int32_t* numComponents);
DLL_SYMBOL void              E_setComponent (struct entity *obj, struct component *data);

#endif /* ECS_ENTITY_H_ */