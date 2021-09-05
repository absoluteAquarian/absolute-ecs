#include "core.h"
#include "utility.h"

size_t g_totalalloc;

DLL_SYMBOL void *malloc_debug(size_t size){
	void *ptr = malloc(size);
	g_totalalloc += size;
	return ptr;
}

DLL_SYMBOL void free_debug(void *ptr, size_t size){
	free(ptr);
	g_totalalloc -= size;
}