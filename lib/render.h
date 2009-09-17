#ifndef __RENDER_H__
#define __RENDER_H__

#include <librsvg/rsvg.h>
#include "memory.h"
#include "mygl.h"

struct render_context_St;
struct render_object_St;

typedef struct {
	struct render_context_St *rcontext;
	struct render_object_St *object;
} render_event_args;

typedef struct render_object_St {
	/** An id that describes this object. */
	char *id;

	/* Custom data for this rendering object. */
	void *data;

	/* Render function. */
	void (*render)(render_event_args *event, float delta);

	void (*free)(render_event_args *event);

	struct render_object_St *parent;

	struct render_object_St **children;
	int child_count;
} render_object;

typedef void (*render_selection_callback)(render_event_args *event, void *data);

typedef struct {
	render_object *object;
	render_selection_callback callback;
	void *data;
} render_selection;

typedef struct render_context_St {
	render_object *object;

	int last_render;

	render_selection selections[1024];
	int selection_size;
} render_context;

render_context *render_context_create();

void render_context_free(render_context *rcontext);

render_object *render_object_create(const char *id);

void render_object_free(render_context *rcontext, render_object *object);

void render_object_add_child(render_object *parent, render_object *child);

void render_object_remove_child(render_object *child);

render_object *render_object_find(render_object *parent, const char *id);

render_object *render_object_find_root(render_object *object);

void render_scene_object(
	render_context *rcontext, render_object *object, float delta);

void render_scene_context(render_context *rcontext);

GLuint render_register_selection_callback(
	render_context *rcontext, render_object *object,
	render_selection_callback callback, void *data);

void render_selection_reset(render_context *rcontext);

void render_process_selections(
	render_context *rcontext, GLint hits, GLuint* selections);

void render_svg_texture(RsvgHandle *h, GLuint texture,
						char *node_name, int width, int height);

void render_rect(float x1, float y1, float x2, float y2, GLuint texture);

#endif /* __RENDER_H__ */
