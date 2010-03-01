#include "../../api/render.h"

typedef struct {
	float red, green, blue;
} render_object_solid_color_data;

static void render_object_solid_color_render(render_event_args *event, float delta) {
	render_object_solid_color_data *i = event->object->data;
	glClearColor(i->red, i->green, i->blue, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void render_object_solid_color_free(render_event_args *event) {
	/* render_object_solid_color_data *i = event->object->data; */
	free(event->object->data);
}

render_object *render_object_solid_color(float red, float green, float blue) {
	render_object *o = render_object_create("background");
	render_object_solid_color_data *d = calloc(1, sizeof(render_object_solid_color_data));
	d->red = red;
	d->green = green;
	d->blue = blue;
	o->data = d;
	o->render = render_object_solid_color_render;
	o->free = render_object_solid_color_free;
	return o;
}
