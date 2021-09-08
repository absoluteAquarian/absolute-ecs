#ifndef ECS_SYSTEM_H_
#define ECS_SYSTEM_H_ 1

#include <stdbool.h>
#include <stdint.h>

#include "errorcode.h"
#include "core.h"
#include "message.h"

struct system_base;
struct system_vtable;

/* The base member for any System.  Contains virtual methods and helpful fields */
struct system_base{
	/* How many of said types there are */
	int32_t               m_validTypesCount;
	/* Where this system is located in a p_systemTable in a (struct world) */
	int32_t               m_tableIdx;
	/* How many pending messages have been sent to this system */
	int32_t               m_messageCount;
	/* Which types of components this system processes */
	int32_t              *p_validTypes;
	/* The virtual functions */
	struct system_vtable *vtable;
	/* The next System to run.  If NULL, no System will run next and the update cycle will start over again */
	struct system_base   *p_next;
	/* Metadata shared by most structs in this project */
	struct ecs_metadata  *p_meta;
	/* The messages sent from other systems to this one */
	struct message      **p_messages;
};

/* The virtual fuctions for a system */
struct system_vtable{
	/* The update function that's run before applying the system to entities.
	   Parameters: system, world index */
	void (*preUpdate)   (struct system_base*, int32_t);
	/* The update function for entities.
	   Parameters: system, world index, entity index */
	void (*update)      (struct system_base*, int32_t, int32_t);
	/* The update function that's run after applying the system to entities.
	   Parameters: system, world index */
	void (*postUpdate)  (struct system_base*, int32_t);
	/* The validity function.
	   Parameters: system, world index, entity index */
	bool (*validEntity) (struct system_base*, int32_t, int32_t);
	/* The message-reading function.
	   Parameters: system, world index */
	void (*readMessages)(struct system_base*, int32_t);
};

extern struct system_base *g_systemStart;
extern size_t             *g_systemSizes;

#ifndef BUILD_DLL
/* System types implemented by the framework */
enum SYSTEM_TYPES{
	SYSTEM_PRINTING = 0,
	SYSTEM_MESSAGE_SEND,
	SYSTEM_MESSAGE_READ,
	
	SYSTEM_COUNT
};
#endif /* BUILD_DLL */

           enum ERRCODE S_init         (int32_t i_worldIdx, int32_t i_capacity);
           int32_t      S_findFreeIndex(int32_t i_worldIdx);
DLL_SYMBOL int32_t      S_create       (int32_t i_worldIdx, int32_t i_type, size_t s_allocSize);
           void         S_run          (int32_t i_systemIdx, int32_t i_worldIdx);
           bool         S_validEntity  (struct system_base *p_system, int32_t i_worldIdx, int32_t i_entityIdx);
DLL_SYMBOL void         S_insert       (struct system_base *p_system, int32_t i_typeToSortAfter);
           void         S_readMessages (struct system_base *p_system);
DLL_SYMBOL void         S_addMessage   (struct system_base *p_system, struct message *p_message);

#endif /* ECS_SYSTEM_H_ */