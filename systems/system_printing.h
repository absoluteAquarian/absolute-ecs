#ifndef ECS_SYSTEMS_PRINTING_H_
#define ECS_SYSTEMS_PRINTING_H_

#include <stdbool.h>
#include <stdint.h>

struct system_printing;

struct system_printing{
	/* system_base must be the first member */
	struct system_base super;
	
	char* p_name;
};

struct system_printing *S_createPrintingSystem (int32_t i_worldIdx, char* name);
void                    S_printingSystem_Update(struct system_base *p_system, int32_t i_worldIdx, int32_t i_entityIdx);

#endif /* ECS_SYSTEMS_PRINTING_H_ */