#ifndef ECS_WORLD_H_
#define ECS_WORLD_H_

#include <stdbool.h>
#include <stdint.h>

#include "component.h"
#include "core.h"
#include "entity.h"

struct world{
	struct ecs_metadata *p_meta;
	
	struct entity **p_entityTable;
	struct component_table **p_componentTable;
	int32_t m_etCapacity;
	int32_t m_ctCapacity;
	
	int32_t m_tableIdx;
};

extern struct world **g_worlds;
extern int32_t g_worldCount;
extern struct world *g_defaultWorld;

enum ERRCODE W_init                 (int32_t capacity);
int32_t      W_findFreeIndex        ();
int32_t      W_create               ();
enum ERRCODE W_destroy              (struct world *p_world);
bool         W_hasEntity            (struct world *p_world, int32_t i_entityID, int32_t* p_entityIdx);
int32_t      W_findEntityParentWorld(struct entity *p_entity);

#endif /* ECS_WORLD_H_ */