#include <librsvg/rsvg.h>
#include <stdlib.h>
#include "mygl.h"
#include "render_mainmenu.h"

typedef struct {
	GLuint textures[1];
} internal;

void render_object_mainmenu_render(
	render_context *rcontext, render_object *object, float delta) {
	internal *i = object->data;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	render_rect(-150.0f, 150.0f, 150.0f, -150.0f, i->textures[0]);

	render_rect(-120.0f, 85.0f, 40.0f, 55.0f, i->textures[1]);
	render_rect(-120.0f, 50.0f, 40.0f, 20.0f, i->textures[2]);
	render_rect(-120.0f, 15.0f, 40.0f, -15.0f, i->textures[3]);
	render_rect(-120.0f, -20.0f, 40.0f, -50.0f, i->textures[4]);

	glDisable(GL_BLEND);
}

render_object *render_object_mainmenu(mem_context *context) {
	GError* e = NULL;
	RsvgHandle* h;
	internal *i = mem_alloc(context, sizeof(internal));
	render_object *o = render_object_create(context, "mainmenu");
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
