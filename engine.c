#include "engine.h"
#include "system.h"
#include "world.h"

DLL_SYMBOL void AECS_Init(){
	/* Create the initial worlds */
#ifdef DEBUG_INFO
	puts("Creating initial worlds...");
#endif
	W_init(4);
	
	/* Create the default world, ID 0 */
	W_create();
	
	g_componentCount = -1;
}

DLL_SYMBOL void AECS_Run(){
	for(struct system_base *p_run = g_systemStart; p_run; p_run = p_run->p_next){
		S_readMessages(p_run);
		
		S_run(p_run->m_tableIdx, g_defaultWorld->m_tableIdx);
	}
}