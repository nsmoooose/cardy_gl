#include "string.h"
#include "render.h"

render_context *render_context_create(mem_context *context) {
	return mem_alloc(context, sizeof(render_context));
}

render_object *render_object_create(mem_context *context, char *id) {
	render_object *o = mem_alloc(context, sizeof(render_object));
	if(id != 0) {
		o->id = strdup(id);
		mem_attach(context, o->id);
	}
	return o;
}

void render_object_add_child(mem_context *context, render_object *parent, render_object *child) {
	render_object **old = parent->children;

	parent->children = mem_alloc(context, (parent->child_count + 1) *
								 sizeof(render_object*));
	parent->child_count++;

	if(old) {
		memcpy(parent->children, old, sizeof(render_object*) *
			   (parent->child_count - 1));
		mem_free(context, old);
	}
	parent->children[parent->child_count - 1] = child;
}

void render_object_remove_child(
	mem_context *context, render_object *parent, render_object *child) {
}

render_object *render_object_find(render_object *root, char *id) {
	return 0;
}

void render_scene_object(
	render_context *rcontext, render_object *object, float delta) {
	int i;
	object->render(rcontext, object, delta);
	for(i=0;i<object->child_count;++i) {
		render_scene_object(rcontext, object->children[i], delta);
	}
}

void render_scene_context(render_context *rcontext, float delta) {
	render_selection_reset(rcontext);
	if(rcontext->object) {
		render_scene_object(rcontext, rcontext->object, delta);
	}
}

GLuint render_register_selection_callback(
	render_context *rcontext, render_selection_callback callback, void *data) {
	GLuint i = rcontext->selection_size;
	rcontext->selections[i].callback = callback;
	rcontext->selections[i].data = data;
	rcontext->selection_size++;
	return i;
}

void render_selection_reset(render_context *rcontext) {
	memset(rcontext->selections, 0, sizeof(rcontext->selections));
	rcontext->selection_size = 0;
}

void render_process_selections(
	render_context *rcontext, GLint hits, GLuint* selections) {
	int index, hit, records;
	GLuint last_hit=0;

	for(index=0, hit=0;hit<hits;++hit) {
		records = selections[index];
		last_hit = selections[index + 2 + records];
		index += records + 3;
	}

	rcontext->selections[last_hit].callback(rcontext->selections[last_hit].data);
}
