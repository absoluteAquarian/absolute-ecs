#ifndef UTILITY_H_
#define UTILITY_H_ 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "core.h"
#include "errorcode.h"

#define STRINGIFY(x) #x
#define STRINGIFY_VALUE(x) STRINGIFY(x)

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE*)0)->ELEMENT))

#define REPORT_ADDR(x) do{ printf("ADDRESS FOR VALUE \"" STRINGIFY(x) "\": 0x%08x\n", &x); }while(0)

#define ALLOC(VALUE, SCALAR) do{ \
		VALUE = malloc_debug(sizeof *(VALUE) * (SCALAR)); \
		memset(VALUE, 0, sizeof *(VALUE) * (SCALAR));\
	}while(0)

#define ENSURE_ALLOC(VALUE, SCALAR) do{ \
		if(!(VALUE) || (VALUE) == (void*)MEM_MALLOC_HEAP_GUARD || (VALUE) == (void*)MEM_MALLOC_UNINITIALIZED){ \
			VALUE = malloc_debug(sizeof *(VALUE) * (SCALAR)); \
			memset(VALUE, 0, sizeof *(VALUE) * (SCALAR)); \
		} \
	}while(0)

#define ALLOC_DIRECT(VALUE, SIZE) do{ \
		VALUE = malloc_debug(SIZE); \
		memset(VALUE, 0, (SIZE)); \
	}while(0)

#define ENSURE_ALLOC_DIRECT(VALUE, SIZE) do{ \
		if(!(VALUE)){ \
			VALUE = malloc_debug(SIZE); \
			memset(VALUE, 0, (SIZE)); \
		} \
	}while(0)

extern size_t g_totalalloc;

DLL_SYMBOL void *malloc_debug(size_t size);
DLL_SYMBOL void  free_debug  (void *ptr, size_t size);
DLL_SYMBOL int   AECS_kbhit  ();
DLL_SYMBOL char  AECS_getch  ();

#endif /* UTILITY_H_ */