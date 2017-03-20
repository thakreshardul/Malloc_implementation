#ifndef HEADER_GUARD
#define HEADER_GUARD                        
#include "libmalloc.h"
#endif

void free(void *ptr)
{
	block block_start;
	block_start = get_used_block_details(ptr);
	if (block_start.startAddr == NULL)
		return;
	int index, size = block_start.size;
	if (size <= 16) 
		index = 0;
	else if (size <= 64)
		index = 1;
	else if (size <= 512)
		index = 2;
	else
		index = 3;
	
	if (index < 3)
	{
		int empty_free = infreelist(index);
		freelist[index][empty_free] = block_start;
		return;
	}
	else
	{
		munmap(block_start.startAddr, block_start.size);
		return;		
	}
	
}