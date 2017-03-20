#ifndef HEADER_GUARD
#define HEADER_GUARD                        
#include "libmalloc.h"
#endif

void *malloc(int size)
{
	if(arena[0].startAddr == NULL)
		init_arena();
	if(arena[0].startAddr == NULL)
		return NULL;

	int index;
	if (size <= 16) 
	{
		size = 16;
		index = 0;
	}
	else if (size <= 64)
	{
		size = 64;
		index = 1;
	}
	else if (size <= 512)
	{
		size = 512;
		index = 2;
	}
	else
	{
		size = size;
		index = 3;
	}
	void *ret = request_memory(size, index);
	return ret;

}