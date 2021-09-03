#ifndef ECS_CORE_H_
#define ECS_CORE_H_

#include <stdbool.h>

struct ecs_metadata{
	bool m_destroyed;
	int32_t m_id;
};

#endif /* ECS_CORE_H_ */