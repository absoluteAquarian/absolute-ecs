#ifndef ERRORCODE_H_
#define ERRORCODE_H_

#include <stdio.h>
#include <stdlib.h>

#include "core.h"

#define log_err_lf(errcode) log_err(errcode, __LINE__, __FILE__)

#define log_err_msg_lf(msg) log_err_msg(msg, __LINE__, __FILE__)

enum ERRCODE{
	RESULT_OK,
	RESULT_INIT_ENTITY_TABLE_CAPACITY,
	RESULT_INIT_COMPONENT_TABLE_CAPACITY,
	RESULT_NULL_ENTITY,
	RESULT_ENTITY_DESTROYED,
	RESULT_INVALID_COMPONENT_TYPE,
	RESULT_ENTITY_TABLE_UNINITIALIZED,
	RESULT_COMPONENT_TABLE_UNINITIALIZED,
	RESULT_WORLD_UNINITIALIZED,
	RESULT_INIT_WORLD_TABLE_CAPACITY,
	RESULT_ENTITY_NOT_BOUND_TO_WORLD,
	RESULT_WORLD_INVALID_INDEX,
	RESULT_NULL_METADATA,
	RESULT_WORLD_TABLE_UNINITIALIZED,
	RESULT_NULL_WORLD,
	RESULT_NULL_SYSTEM,
	RESULT_SYSTEM_UNINTIALIZED,
	RESULT_SYSTEM_TABLE_UNINITIALIZED,
	RESULT_INIT_SYSTEM_TABLE_CAPACITY,
	RESULT_NOT_REGISTERED_SYSTEM_TYPE,
	RESULT_CYCLICAL_SYSTEM_STUCTURE,
	RESULT_NULL_SYSTEM_MESSAGES,
	RESULT_SYSTEM_DESTROYED,
	RESULT_NULL_MESSAGE,
	RESULT_INVALID_SYSTEM_INDEX
};

enum MEMCODE{
	/* Used by Microsoft's debug HeapAlloc() to mark "no man's land" guard bytes after allocated heap memory. */
	MEM_MALLOC_HEAP_GUARD = 0xabababab,
	/* "Bad food", Used by Microsoft's debug HeapAlloc() to mark uninitialized allocated heap memory */
	MEM_MALLOC_UNINITIALIZED = 0xbaadf00d,
	/* Used by Microsoft's C++ debugging runtime library and many DOS environments to mark uninitialized stack memory.
	   'CC' resembles the opcode of the INT 3 debug breakpoint interrupt on x86 processors. */
	MEM_STACK_UNITIALIZED = 0xcccccccc,
	/* Used by Microsoft's C/C++ debug malloc() function to mark uninitialized heap memory, usually returned from HeapAlloc() */
	MEM_HEAP_UNINITIALIZED = 0xcdcdcdcd,
	/* Used by MicroQuill's SmartHeap and Microsoft's C/C++ debug free() function to mark freed heap memory */
	MEM_DEBUG_HEAP_FREE = 0xdddddddd,
	/* "Dead Dead" indicates that the user deliberately initiated a crash dump from either the kernel debugger or the keyboard. */
	MEM_FORCED_CRASH = 0xdeaddead,
	/* Used by Microsoft's C/C++ debug malloc() function to mark "no man's land" guard bytes before and after allocated heap memory,
	   and some debug Secure C-Runtime functions implemented by Microsoft (e.g. strncat_s) */
	MEM_DEBUG_MALLOC_HEAP_GUARD = 0xfdfdfdfd,
	/* "Fee fee", Used by Microsoft's debug HeapFree() to mark freed heap memory.
	   Some nearby internal bookkeeping values may have the high word set to FEEE as well. */
	MEM_HEAP_FREE = 0xfeeefeee
};

DLL_SYMBOL void log_err    (enum ERRCODE errcode, int64_t line, const char* file);
DLL_SYMBOL void log_err_msg(const char* p_msg, int64_t line, const char* file);

#endif /* ERRORCODE_H_ */