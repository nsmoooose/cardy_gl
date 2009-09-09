#include <librsvg/rsvg.h>
#include <stdlib.h>
#include "mygl.h"
#include "render_mainmenu.h"
#include "render_solitaire.h"
#include "solitaire_theidiot.h"
#include "solitaire_maltesercross.h"
#include "solitaire_noname1.h"

const char* render_object_mainmenu_id = "mainmenu";

typedef struct {
	GLuint textures[5];
} internal;

void sol_theidiot_callback(render_object *object, void *data) {
	render_object *menu = data;
	render_object_add_child(menu->parent,
							render_object_solitaire(solitaire_theidiot));
	render_object_remove_child(menu->parent, menu);
}

void sol_malteser_callback(render_object *object, void *data) {
	render_object *menu = data;
	render_object_add_child(menu->parent,
							render_object_solitaire(solitaire_maltesercross));
	render_object_remove_child(menu->parent, menu);
}

void sol_noname1_callback(render_object *object, void *data) {
	render_object *menu = data;
	render_object_add_child(menu->parent,
							render_object_solitaire(solitaire_noname1));
	render_object_remove_child(menu->parent, menu);
}

void sol_pyramid_callback(render_object *object, void *data) {
}

void render_object_mainmenu_render(
	render_context *rcontext, render_object *object, float delta) {
	internal *i = object->data;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	render_rect(-150.0f, 150.0f, 150.0f, -150.0f, i->textures[0]);

	glPushName(render_register_selection_callback(
				   rcontext, object, sol_theidiot_callback, object));
	render_rect(-120.0f, 85.0f, 40.0f, 55.0f, i->textures[1]);
	glPopName();

	glPushName(render_register_selection_callback(
				   rcontext, object, sol_malteser_callback, object));
	render_rect(-120.0f, 50.0f, 40.0f, 20.0f, i->textures[2]);
	glPopName();

	glPushName(render_register_selection_callback(
				   rcontext, object, sol_noname1_callback, object));
	render_rect(-120.0f, 15.0f, 40.0f, -15.0f, i->textures[3]);
	glPopName();

	glPushName(render_register_selection_callback(
				   rcontext, object, sol_pyramid_callback, object));
	render_rect(-120.0f, -20.0f, 40.0f, -50.0f, i->textures[4]);
	glPopName();

	glDisable(GL_BLEND);
}

render_object *render_object_mainmenu() {

	GError* e = NULL;
	RsvgHandle* h;
	internal *i = calloc(1, sizeof(internal));
	render_object *o = render_object_create(render_object_mainmenu_id);
	o->data = i;
	o->render = render_object_mainmenu_render;

	/* Load all the textures that is needed to render. */
	h = rsvg_handle_new_from_file("resources/images/mainmenu.svg", &e);
	if(e != NULL) {
		fprintf(stderr, "Failed to load mainmenu.svg\n");
		exit(1);
	}

	glGenTextures(5, i->textures);
	render_svg_texture(h, i->textures[0], "#mainmenu", 512, 512);
	render_svg_texture(h, i->textures[1], "#sol_idiot", 512, 64);
	render_svg_texture(h, i->textures[2], "#sol_maltesercross", 512, 64);
	render_svg_texture(h, i->textures[3], "#sol_noname1", 512, 64);
	render_svg_texture(h, i->textures[4], "#sol_pyramid", 512, 64);
	return o;
}
