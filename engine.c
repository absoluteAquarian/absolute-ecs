#include "engine.h"
#include "system.h"
#include "world.h"
#include "systems/system_printing.h"

DLL_SYMBOL void AECS_Init(){
	/* Create the initial worlds */
#ifdef DEBUG_INFO
	puts("Creating initial worlds...");
#endif
	W_init(4);
	
	/* Create the default world, ID 0 */
	W_create();
	
	g_componentCount = -1;
	
	int32_t printSystem = S_createPrintingSystem(g_defaultWorld->m_tableIdx, "Jeff")->super.m_tableIdx;
	
	struct system_base *p_system = *(g_defaultWorld->p_systemTable + printSystem);
	
	/* The type to sort after doesn't matter here since no systems have been added */
	S_insert(p_system, 0);
}

DLL_SYMBOL void AECS_Run(){
	struct system_base *p_run = g_systemStart;
	
	do{
		S_run(p_run->m_tableIdx, g_defaultWorld->m_tableIdx);
		
		p_run = p_run->p_next;
	}while(p_run);
}