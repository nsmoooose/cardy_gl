#ifndef __MEMORY_H__
#define __MEMORY_H__

typedef struct {
	void** blocks;
	int size;
} mem_context;

mem_context *mem_create_context();
void mem_free_context(mem_context *context);
void* mem_alloc(mem_context *context, int size);
void mem_free(mem_context *context, void *mem);

#endif /* __MEMORY_H__ */
