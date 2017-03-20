#ifndef HEADER_GUARD
#define HEADER_GUARD                        
#include "libmalloc.h"
#endif


void *realloc(void* ptr, int new_size)
{
  void *new;
  block block_start = get_used_block_details(ptr);
  
  if (new_size == 0) {
    free(ptr);
    return NULL;
  }

  new = malloc(new_size);

  if (ptr == NULL) return new;
 
  memcpy(new, ptr, block_start.size);

  free(ptr);

  return new;
}

