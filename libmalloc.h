#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define MAX_PAGE_NOS 50

typedef struct block_t block;

struct block_t
{
	void   *startAddr;
	int    size;
};

static __thread block arena[MAX_PAGE_NOS];
static __thread block freelist[4][1024];
static __thread block usedlist[4][1024];


block get_used_block_details(ptr)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			if (usedlist[i][j].startAddr == ptr)
			{
				block temp = usedlist[i][j];
				usedlist[i][j].size = 0;
				return temp;
			}
		}
	}
}

int infreelist(int index)
{
	for (int i = 0; i < 1024; i++)
	{
		if (freelist[index][i].size == 0)
			return i;
		else
			return -1;
	}
}

int getusedlist(int index)
{
	for (int i = 0; i < 1024; i++)
	{
		if (usedlist[index][i].size == 0)
			return i;
		else
			return -1;
	}
}
void init_arena()
{
	arena[0].startAddr = sbrk(PAGE_SIZE);
	arena[0].size = PAGE_SIZE;
}

void *request_memory(int size, int index)
{
	int free_empty = infreelist(index);
	int used_empty = getusedlist(index);
	if (index < 3)
	{
		if (!free_empty)
		{
			int available_in_arena = arena[0].size;
			int i = 0, isnull = 0;
			while((available_in_arena < size) && (i < MAX_PAGE_NOS))
			{
				if (arena[i].startAddr == NULL)
				{
					isnull = 1;
					break;
				}
				i += 1;
				available_in_arena = arena[i].size;
			}

			if (i == MAX_PAGE_NOS)
			{
				fprintf(stderr, "CANNOT GIVE MORE MEMORY\n");
				return NULL;
			}

			if (available_in_arena >= size)
			{
				block new_block;
				new_block.startAddr = arena[i].startAddr;
				new_block.size = size;
				usedlist[index][used_empty] = new_block;
				
				arena[i].size = arena[i].size - new_block.size;
				arena[i].startAddr = arena[i].startAddr + new_block.size;
				return new_block.startAddr;
			}

			if (isnull)
			{
				arena[i].startAddr = sbrk(PAGE_SIZE);
				arena[i].size = PAGE_SIZE;
				
				block new_block;
				new_block.startAddr = arena[i].startAddr;
				new_block.size = size;
				usedlist[index][used_empty] = new_block;
				
				arena[i].size = arena[i].size - new_block.size;
				arena[i].startAddr = arena[i].startAddr + new_block.size;
				return new_block.startAddr;

			}
		}
		else
		{
			block new_block;
			new_block.startAddr = freelist[index][free_empty].startAddr;
			new_block.size = size;
			usedlist[index][used_empty] = new_block;
			freelist[index][free_empty].size = 0;
			return new_block.startAddr; 
		}
	}
	else
	{
		void *ret = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		block new_block;
		new_block.startAddr = ret;
		new_block.size = size;
		usedlist[index][used_empty] = new_block;
		return ret;
	}
}
