#include <librsvg/rsvg.h>
#include <stdlib.h>
#include "../api/render_widget.h"
#include "render_topmenu.h"

const char* render_object_topmenu_id = "topmenu";
#define g_topmenu_texture_count 1

typedef struct {
	GLuint textures[g_topmenu_texture_count];
} render_object_topmenu_data;

static void topmenu_quit_callback(render_event_args *event, void *data) {
	exit(0);
}

static void render_object_topmenu_render(render_event_args *event, float delta) {
	render_object_topmenu_data *i = event->object->data;

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -500.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPushName(
		render_register_selection_callback(
			event->rcontext, event->object, topmenu_quit_callback, 0));
	render_rect(230.0f, 180.0f, 250.0f, 200.0f, i->textures[0]);
	glPopName();
}

static void render_object_topmenu_free(render_event_args *event) {
	render_object_topmenu_data *i = event->object->data;
	glDeleteTextures(g_topmenu_texture_count, i->textures);
	free(event->object->data);
}

render_object *render_object_topmenu() {
	GError* e = NULL;
	RsvgHandle* h;
	render_object *o = render_object_create(render_object_topmenu_id);
	render_object_topmenu_data *i = calloc(1, sizeof(render_object_topmenu_data));
	o->data = i;
	o->free = render_object_topmenu_free;
	o->render = render_object_topmenu_render;

	/* Load all the textures that is needed to render. */
	h = rsvg_handle_new_from_file("resources/images/topmenu.svg", &e);
	if(e != NULL) {
		fprintf(stderr, "Failed to load topmenu.svg\n");
		exit(1);
	}

	glGenTextures(g_topmenu_texture_count, i->textures);
	render_svg_texture(h, i->textures[0], "#quit", 64, 64);
	return o;
}

void render_object_topmenu2(render_object *parent) {
	render_object *menu, *options, *quit;
	render_object *window = widget_window("topmenu");
	widget_style_set_pos(window, 0.0f, 0.0f);
	widget_style_set_size(window, 64.0f * 3 + 4 * 4.0f, 72.0f);
	render_object_add_child(parent, window);

	menu = widget_button(0);
	widget_style_set_pos(menu, 4.0f, 4.0f);
	widget_style_set_size(menu, 64.0f, 64.0f);
	render_object_add_child(window, menu);

	options = widget_button(0);
	widget_style_set_pos(options, 72.0f, 4.0f);
	widget_style_set_size(options, 64.0f, 64.0f);
	render_object_add_child(window, options);

	quit = widget_button(0);
	widget_style_set_pos(quit, 140.0f, 4.0f);
	widget_style_set_size(quit, 64.0f, 64.0f);
	render_object_add_child(window, quit);
}
