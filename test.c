#include <assert.h>
#include <stdio.h>


int main(int argc, char **argv)
{
  int size = 1200;
  void *mem = malloc(size);
  printf("Successfully malloc'd %d bytes at addr %p\n", size, mem);
  assert(mem != NULL);
  free(mem);
  printf("Successfully free'd %d bytes from addr %p\n", size, mem);
  return 0;
}