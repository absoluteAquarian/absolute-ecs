#include <stdint.h>

#include "core.h"
#include "message.h"
#include "utility.h"

DLL_SYMBOL struct message *M_create(int32_t i_srcSystem, int32_t i_destSystem, void* p_data, const char* p_typeName){
	struct message* msg;
	ALLOC(msg, 1);
	
	msg->m_srcSystem = i_srcSystem;
	msg->m_destSystem = i_destSystem;
	ALLOC(msg->p_data, 1);
	
	msg->p_data->p_content = p_data;
	msg->p_data->p_typeName = p_typeName;
	ALLOC(msg->vtable);
	
	return msg;
}