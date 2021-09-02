#ifndef ENTITY_H_
#define ENTITY_H_

#include <stdbool.h>
#include <stdint.h>

#include "component.h"
#include "errorcode.h"

struct entity;
struct entity_vtable;

struct entity_vtable{
	bool (*has_component)(struct entity *obj, enum COMPONENT_TYPE type);
	struct component *(*get_component)(struct entity *obj, enum COMPONENT_TYPE type);
	void (*set_component)(struct entity *obj, struct component *data);
};

struct entity{
	struct entity_vtable *vtable;
	bool m_destroyed;
	int32_t m_id;
	int32_t m_tableidx;
};

enum ERRCODE init_entity_table(int32_t capacity);
int32_t find_free_entity();
int32_t create_entity();
enum ERRCODE destroy_entity(struct entity *obj);
bool entity_has_component(struct entity *obj, enum COMPONENT_TYPE type);
struct component *entity_get_component(struct entity *obj, enum COMPONENT_TYPE type);
void entity_set_component(struct entity *obj, struct component *data);

/* Extern here... */
extern struct entity* *g_entitytable;
extern int32_t g_entitytable_capacity;

#endif /* ENTITY_H_ */