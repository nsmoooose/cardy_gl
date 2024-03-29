#pragma once

#include <stdbool.h>
#include <librsvg/rsvg.h>
#include "memory.h"
#include "mygl.h"

struct render_context_St;
struct render_object_St;

#define QUEUE_FREE_SIZE 50

typedef struct {
	struct render_context_St *rcontext;
	struct render_object_St *object;
} render_event_args;

typedef struct render_object_St {
	/** An id that describes this object. */
	char *id;

	/* Custom data for this rendering object. */
	void *data;

	/* Total time spent rendering. */
	float render_time;
	/* Flag indicating if the first frame has been rendered. */
	bool first_frame_rendered;

	/* Render function. */
	void (*render)(render_event_args *event, float delta);
	void (*post_render)(render_event_args *event, float delta);

	void (*free)(render_event_args *event);

	bool (*keyboard_down)(render_event_args *event, unsigned char key,
	                      int modifiers, int x, int y);
	bool (*keyboard_up)(render_event_args *event, unsigned char key,
	                    int modifiers, int x, int y);

	bool (*keyboard_special_down)(render_event_args *event, int key,
	                              int modifiers, int x, int y);
	bool (*keyboard_special_up)(render_event_args *event, int key,
	                            int modifiers, int x, int y);

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

typedef struct {
	int x, y;
	int width, height;

	GLint viewport[4];
} render_pick;

typedef struct render_context_St {
	render_object *object;

	int last_render;

	render_pick *pick;

	render_selection selections[1024];
	int selection_size;

	render_object *queue_free[QUEUE_FREE_SIZE];
} render_context;

render_context *render_context_create(void);

void render_context_free(render_context *rcontext);

render_object *render_object_create(const char *id);

void render_object_free(render_context *rcontext, render_object *object);
void render_object_queue_free(render_context *rcontext, render_object *object);
void render_object_queue_free_children(render_context *rcontext,
                                       render_object *object);

void render_object_add_child(render_object *parent, render_object *child);

void render_object_remove_child(render_object *child);

render_object *render_object_find(render_object *parent, const char *id);

render_object *render_object_find_root(render_object *object);

void render_scene_object(render_context *rcontext, render_object *object,
                         float delta);

void render_scene_context(render_context *rcontext);

GLuint render_register_selection_callback(render_context *rcontext,
                                          render_object *object,
                                          render_selection_callback callback,
                                          void *data);

void render_selection_reset(render_context *rcontext);

void render_process_selections(render_context *rcontext, GLint hits,
                               GLuint *selections);

void render_process_keyboard_down(render_context *rcontext, unsigned char key,
                                  int modifiers, int x, int y);
void render_process_keyboard_up(render_context *rcontext, unsigned char key,
                                int modifiers, int x, int y);

void render_process_keyboard_special_down(render_context *rcontext, int key,
                                          int modifiers, int x, int y);
void render_process_keyboard_special_up(render_context *rcontext, int key,
                                        int modifiers, int x, int y);

RsvgHandle *render_svg_open(const char *path);

void render_svg_close(RsvgHandle *h);

void render_svg_texture(RsvgHandle *h, GLuint texture, char *node_name,
                        int width, int height);

void render_rect(float x1, float y1, float x2, float y2, GLuint texture);
