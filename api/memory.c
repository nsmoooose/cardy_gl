#include <stdlib.h>
#include <string.h>
#include "api/memory.h"

static int first_free(mem_context *c) {
	int index;
	for (index = 0; index < c->size; ++index) {
		if (c->blocks[index] == 0) {
			return index;
		}
	}
	return -1;
}

mem_context *mem_context_create(void) {
	mem_context *c = calloc(1, sizeof(mem_context));
	c->blocks = calloc(1000, sizeof(void *));
	c->size = 1000;
	return c;
}

void mem_context_free(mem_context *context) {
	int index;
	for (index = 0; index < context->size; ++index) {
		if (context->blocks[index] != 0) {
			free(context->blocks[index]);
		}
	}
	free(context->blocks);
	free(context);
}

void mem_attach(mem_context *context, void *mem) {
	int index = first_free(context);
	if (index == -1) {
		/* TODO: Something */
		exit(0);
	}
	context->blocks[index] = mem;
}

void *mem_alloc(mem_context *context, int size) {
	int index;
	void *old_blocks, *data = calloc(1, size);

	index = first_free(context);
	if (index == -1) {
		int old_size = context->size;
		old_blocks = context->blocks;
		context->blocks = calloc(old_size * 2, sizeof(void *));
		context->size = old_size * 2;

		memcpy(context->blocks, old_blocks, old_size * sizeof(void *));
		free(old_blocks);
	}

	context->blocks[first_free(context)] = data;
	return data;
}

void mem_free(mem_context *context, void *mem) {
	int index;
	for (index = 0; index < context->size; ++index) {
		if (context->blocks[index] == mem) {
			context->blocks[index] = 0;
			free(mem);
			return;
		}
	}
}
