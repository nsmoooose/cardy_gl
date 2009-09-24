#include <librsvg/rsvg.h>
#include <stdlib.h>
#include "../api/mygl.h"
#include "render_solitaire.h"
#include "render_mainmenu.h"
#include "solitaire_theidiot.h"
#include "solitaire_maltesercross.h"
#include "solitaire_noname1.h"

const char* render_object_mainmenu_id = "mainmenu";
#define g_mainmenu_texture_count 5

typedef struct {
	GLuint textures[g_mainmenu_texture_count];
	float time_start;
	float time_elapsed;
} internal;

static void remove_existing_solitaire(render_context *rcontext,
									  render_object *object) {
	render_object *root = render_object_find_root(object);
	render_object *sol = render_object_find(root, render_object_solitaire_id);
	if(sol != 0) {
		render_object_free(rcontext, sol);
	}
}

static void sol_theidiot_callback(render_event_args *event, void *data) {
	remove_existing_solitaire(event->rcontext, event->object);
	render_object_add_child(event->object->parent,
							render_object_solitaire(solitaire_theidiot));
	render_object_free(event->rcontext, event->object);
}

static void sol_malteser_callback(render_event_args *event, void *data) {
	remove_existing_solitaire(event->rcontext, event->object);
	render_object_add_child(event->object->parent,
							render_object_solitaire(solitaire_maltesercross));
	render_object_free(event->rcontext, event->object);
}

static void sol_noname1_callback(render_event_args *event, void *data) {
	remove_existing_solitaire(event->rcontext, event->object);
	render_object_add_child(event->object->parent,
							render_object_solitaire(solitaire_noname1));
	render_object_free(event->rcontext, event->object);
}

static void sol_pyramid_callback(render_event_args *event, void *data) {
}

static void render_object_mainmenu_render(render_event_args *event, float delta) {
	internal *i = event->object->data;
	float opacity;
	float time;

	if(i->time_start == 0.0f) {
		/* This is the first render of the main menu. Since it took some time
		   to generate the main menu image from the svg file we will not calculate
		   any rendering from here. */
		i->time_start = delta;
		i->time_elapsed = delta;
		return;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -500.0f);

	i->time_elapsed += delta;
	time = i->time_elapsed - i->time_start;
	opacity = time * 2 > 1.0 ? 1.0 : time * 2;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, opacity);
	render_rect(-150.0f, 150.0f, 150.0f, -150.0f, i->textures[0]);

	glPushName(render_register_selection_callback(
				   event->rcontext, event->object, sol_theidiot_callback, 0));
	render_rect(-120.0f, 85.0f, 40.0f, 55.0f, i->textures[1]);
	glPopName();

	glPushName(render_register_selection_callback(
				   event->rcontext, event->object, sol_malteser_callback, 0));
	render_rect(-120.0f, 50.0f, 40.0f, 20.0f, i->textures[2]);
	glPopName();

	glPushName(render_register_selection_callback(
				   event->rcontext, event->object, sol_noname1_callback, 0));
	render_rect(-120.0f, 15.0f, 40.0f, -15.0f, i->textures[3]);
	glPopName();

	glPushName(render_register_selection_callback(
				   event->rcontext, event->object, sol_pyramid_callback, 0));
	render_rect(-120.0f, -20.0f, 40.0f, -50.0f, i->textures[4]);
	glPopName();

	glDisable(GL_BLEND);
}

static void render_object_mainmenu_free(render_event_args *event) {
	internal *i = event->object->data;
	glDeleteTextures(g_mainmenu_texture_count, i->textures);
	free(event->object->data);
}

render_object *render_object_mainmenu() {

	GError* e = NULL;
	RsvgHandle* h;
	internal *i = calloc(1, sizeof(internal));
	render_object *o = render_object_create(render_object_mainmenu_id);
	o->data = i;
	o->render = render_object_mainmenu_render;
	o->free = render_object_mainmenu_free;

	/* Load all the textures that is needed to render. */
	h = rsvg_handle_new_from_file("resources/images/mainmenu.svg", &e);
	if(e != NULL) {
		fprintf(stderr, "Failed to load mainmenu.svg\n");
		exit(1);
	}

	glGenTextures(g_mainmenu_texture_count, i->textures);
	render_svg_texture(h, i->textures[0], "#mainmenu", 512, 512);
	render_svg_texture(h, i->textures[1], "#sol_idiot", 512, 64);
	render_svg_texture(h, i->textures[2], "#sol_maltesercross", 512, 64);
	render_svg_texture(h, i->textures[3], "#sol_noname1", 512, 64);
	render_svg_texture(h, i->textures[4], "#sol_pyramid", 512, 64);
	return o;
}
