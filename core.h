#ifndef ECS_CORE_H_
#define ECS_CORE_H_

#include <stdint.h>

/* If the DLL is being built, make  export the indicated functions.  Otherwise, just treat it as an empty token */
#ifdef BUILD_DLL
 #define DLL_SYMBOL __declspec(dllexport)
#else
 #define DLL_SYMBOL
#endif

#include <stdbool.h>

struct ecs_metadata{
	bool m_destroyed;
	int32_t m_id;
};

#endif /* ECS_CORE_H_ */