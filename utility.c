#include "utility.h"

size_t g_totalalloc;

void *malloc_debug(size_t size){
	void *ptr = malloc(size);
	g_totalalloc += size;
	return ptr;
}

void free_debug(void *ptr, size_t size){
	free(ptr);
	g_totalalloc -= size;
}