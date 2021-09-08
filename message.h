#ifndef ECS_MESSAGE_H_
#define ECS_MESSAGE_H_ 1

#include <stdint.h>

struct message;
struct message_content;

struct message{
	int32_t                 m_srcSystem;
	int32_t                 m_destSystem;
	struct message_content *p_data;
};

struct message_content{
	const char* p_typeName;
	void*       p_content;
};

DLL_SYMBOL struct message *M_create(int32_t i_srcSystem, int32_t i_destSystem, void* p_data, const char* p_typeName);

#endif /* ECS_MESSAGE_H_ */