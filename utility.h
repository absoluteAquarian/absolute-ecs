#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdlib.h>
#include <stdio.h>

#define STRINGIFY(x) #x
#define STRINGIFY_VALUE(x) STRINGIFY(x)

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE*)0)->ELEMENT))

#define REPORT_ADDR(x) do{ printf("ADDRESS FOR VALUE \"" STRINGIFY(x) "\": 0x%08x\n", &x); }while(0)
	
#define ALLOC(VALUE, SCALAR) VALUE = malloc_debug(sizeof *VALUE * (SCALAR))
#define ENSURE_ALLOC(VALUE, SCALAR) do{ if(!VALUE) ALLOC(VALUE, SCALAR); }while(0)

extern size_t g_totalalloc;

void *malloc_debug(size_t size);
void free_debug(void *ptr, size_t size);

#endif /* UTILITY_H_ */