#ifndef ECS_WORLD_H_
#define ECS_WORLD_H_

#include <stdbool.h>
#include <stdint.h>

#include "component.h"
#include "core.h"
#include "entity.h"
#include "system.h"

struct world{
	int32_t                  m_etCapacity;
	int32_t                  m_ctCapacity;
	int32_t                  m_stCapacity;
	
	int32_t                  m_tableIdx;
	
	struct ecs_metadata     *p_meta;
	
	struct entity          **p_entityTable;
	struct component_table **p_componentTable;
	struct system_base     **p_systemTable;
};

extern struct world **g_worlds;
extern int32_t        g_worldCount;
extern struct world  *g_defaultWorld;

           enum ERRCODE W_init                 (int32_t capacity);
           int32_t      W_findFreeIndex        ();
DLL_SYMBOL int32_t      W_create               ();
DLL_SYMBOL enum ERRCODE W_destroy              (struct world *p_world);
DLL_SYMBOL bool         W_hasEntity            (struct world *p_world, int32_t i_entityID, int32_t *p_entityIdx);
DLL_SYMBOL bool         W_hasSystem            (struct world *p_world, int32_t i_systemID, int32_t *p_systemIdx);
DLL_SYMBOL int32_t      W_findEntityParentWorld(struct entity *p_entity);
DLL_SYMBOL int32_t      W_findSystemParentWorld(struct system_base *p_system);

#endif /* ECS_WORLD_H_ */