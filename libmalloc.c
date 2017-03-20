#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define MAX_PAGE_NOS 20

typedef struct block_t block;

struct block_t
{
	void   *startAddr;
	int    size;
	block  *next;
};

static __thread block arena[MAX_PAGE_NOS];
static __thread block *freelist[4];
static __thread block *usedlist[4];

void init_arena()
{
	arena[0].startAddr = sbrk(PAGE_SIZE);
	arena[0].size = PAGE_SIZE;
	
	printf("%d\n", arena[0].size);
	printf("%p\n", arena[0].startAddr);
	printf("%p\n", arena[0].next);
}

void *request_memory(int size, int index)
{
	if (index < 3)
	{
		if (!freelist[index])
		{
			int available_in_arena = arena[0].size;
			int i = 0, isnull = 0;
			while((available_in_arena < size) && (i < MAX_PAGE_NOS))
			{
				fprintf(stdout, "IN LOOP %d:%d:%d\n", available_in_arena, size, i);
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
				//fprintf(stderr, "CANNOT GIVE MORE MEMORY\n");
				return NULL;
			}

			if (available_in_arena >= size)
			{
				block *new_block = (block *)arena[i].startAddr;
				new_block->startAddr = arena[i].startAddr;
				new_block->size = size + sizeof(block);
				new_block->next = usedlist[index];
				usedlist[index] = new_block;
				arena[i].size = arena[i].size - size - sizeof(block);
				arena[i].startAddr = arena[i].startAddr + size + sizeof(block);
				fprintf(stdout, "Arena: %d\n", i);
				fprintf(stdout, "new_block start:%p\nnew_block size:%d\nnew_block next:%p\nusedlist:%p\narena size:%d\narena startAddr:%p\n", new_block->startAddr, new_block->size, new_block->next, usedlist[index], arena[i].size, arena[i].startAddr);
				block *t = usedlist[index];
				while(t->next)
				{
					fprintf(stdout, "%d:%p\n", index, t);
					t = t->next;
				}	
				return new_block->startAddr + sizeof(block);
			}

			if (isnull)
			{
				arena[i].startAddr = sbrk(PAGE_SIZE);
				arena[i].size = PAGE_SIZE;
				arena[i].next = NULL;

				block *new_block = (block *)arena[i].startAddr;
				new_block->startAddr = arena[i].startAddr;
				new_block->size = size + sizeof(block);
				new_block->next = usedlist[index];
				usedlist[index] = new_block;
				arena[i].size = arena[i].size - size - sizeof(block);
				arena[i].startAddr = arena[i].startAddr + size + sizeof(block);
				fprintf(stdout, "Arena: %d\n", i);
				fprintf(stdout, "new_block start:%p\nnew_block size:%d\nnew_block next:%p\nusedlist:%p\narena size:%d\narena startAddr:%p\n", new_block->startAddr, new_block->size, new_block->next, usedlist[index], arena[i].size, arena[i].startAddr);
				return new_block->startAddr + sizeof(block);

			}
		}
		else
		{
			block *new_block = (block *)freelist[index]->startAddr;
			new_block->startAddr = freelist[index]->startAddr;
			new_block->size = size + sizeof(block);
			new_block->next = usedlist[index];
			usedlist[index] = new_block;
			freelist[index] = freelist[index]->next;

			block *t = freelist[index];
			while(t->next)
			{
				fprintf(stdout, "free %d:%p\n", index, t);
				t = t->next;
			}
			t = usedlist[index];
			while(t->next)
			{
				fprintf(stdout, "used %d:%p\n", index, t);
				t = t->next;
			}
			return new_block->startAddr + sizeof(block); 
		}
	}
	else
	{
		int total = size + sizeof(block);
		void *ret = mmap(NULL, total, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		block *new_block = (block *)ret;
		new_block->startAddr = ret;
		new_block->size = total;
		new_block->next = usedlist[index];
		usedlist[index] = new_block;
		return ret + sizeof(block);
	}
}