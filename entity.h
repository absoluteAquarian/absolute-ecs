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

enum ERRCODE      E_init         (int32_t i_worldID, int32_t capacity);
int32_t           E_findFreeIndex(int32_t i_worldID);
int32_t           E_create       (int32_t i_worldID);
enum ERRCODE      E_destroy      (struct entity *obj);
bool              E_hasComponent (struct entity *obj, enum COMPONENT_TYPE type);
struct component *E_getComponent (struct entity *obj, enum COMPONENT_TYPE type);
void              E_setComponent (struct entity *obj, struct component *data);

#endif /* ECS_ENTITY_H_ */