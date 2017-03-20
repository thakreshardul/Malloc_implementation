#ifndef HEADER_GUARD
#define HEADER_GUARD                        
#include "libmalloc.h"
#endif

void *calloc(int nmemb, int size)
{
	int total = nmemb * size;

	void *p = malloc(total);
	
	if (!p) return NULL;
	
	return memset(p, 0, total);
}