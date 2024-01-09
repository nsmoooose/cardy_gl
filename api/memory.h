#pragma once

typedef struct {
	void **blocks;
	int size;
} mem_context;

mem_context *mem_context_create();
void mem_context_free(mem_context *context);
void *mem_alloc(mem_context *context, int size);
void mem_attach(mem_context *context, void *mem);
void mem_free(mem_context *context, void *mem);
