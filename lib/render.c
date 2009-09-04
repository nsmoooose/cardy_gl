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
	return 0;
}

void render_object_add_child(mem_context *context, render_object *parent, render_object *child) {
	render_object **old = parent->children;

	parent->children = mem_alloc(context, (parent->child_count + 1) * sizeof(render_object*));
	parent->child_count++;

	if(old) {
		memcpy(parent->children, old, sizeof(render_object*) * (parent->child_count - 1));
		mem_free(context, old);
	}
	parent->children[parent->child_count - 1] = child;
}

void render_object_remove_child(mem_context *context, render_object *parent, render_object *child) {
}

render_object *render_object_find(render_object *root, char *id) {
	return 0;
}

void render_scene_object(render_object *object, float delta) {
	int i;
	object->render(object, delta);
	for(i=0;i<object->child_count;++i) {
		render_scene_object(object->children[i], delta);
	}
}

void render_scene_context(render_context *rcontext, float delta) {
	if(rcontext->object) {
		render_scene_object(rcontext->object, delta);
	}
}
