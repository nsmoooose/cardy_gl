#ifndef __RENDER_H__
#define __RENDER_H__

#include "memory.h"
#include "mygl.h"

struct render_context_St;

typedef struct render_object_St {
	/** An id that describes this object. */
	char *id;

	/* Custom data for this rendering object. */
	void *data;

	/* Render function. */
	void (*render)(struct render_context_St *rcontext,
				   struct render_object_St *object, float delta);

	/* Activate function when user clicks on this object. */
	void (*activate)(struct render_context_St *rcontext,
					 struct render_object_St *object);

	struct render_object_St **children;
	int child_count;
} render_object;

typedef void (*render_selection_callback)(void *data);

typedef struct {
	render_selection_callback *callback;
	void *data;
} render_selection;

typedef struct render_context_St {
	render_object *object;

	render_selection selections[1024];
	int selection_size;
} render_context;

render_context *render_context_create(mem_context *context);

render_object *render_object_create(mem_context *context, char *id);

void render_object_add_child(
	mem_context *context, render_object *parent, render_object *child);

void render_object_remove_child(
	mem_context *context, render_object *parent, render_object *child);

render_object *render_object_find(render_object *root, char *id);

void render_scene_object(
	render_context *rcontext, render_object *object, float delta);

void render_scene_context(render_context *rcontext, float delta);

GLuint render_register_selection_callback(
	render_context *context, render_selection_callback callback, void *data);


#endif /* __RENDER_H__ */
