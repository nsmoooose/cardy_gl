#include <librsvg/rsvg.h>
#include <stdlib.h>
#include "../api/mygl.h"
#include "../api/render_widget.h"
#include "render_solitaire.h"
#include "render_mainmenu.h"
#include "solitaire_theidiot.h"
#include "solitaire_maltesercross.h"
#include "solitaire_noname1.h"

const char* render_object_mainmenu_id = "mainmenu";

static void set_solitaire(render_event_args *event, solitaire_create sol_callback) {
	render_object *placeholder = render_object_find(
		event->rcontext->object, "placeholder");
	render_object *existing_sol = render_object_find(
		event->rcontext->object, render_object_solitaire_id);

	if(existing_sol) {
		render_object_free(event->rcontext, existing_sol);
	}
	render_object_add_child(placeholder, render_object_solitaire(sol_callback));

	render_object_free(event->rcontext, event->object->parent);
}

static void sol_theidiot_callback(render_event_args *event, void *data) {
	set_solitaire(event, solitaire_theidiot);
}

static void sol_malteser_callback(render_event_args *event, void *data) {
	set_solitaire(event, solitaire_maltesercross);
}

static void sol_noname1_callback(render_event_args *event, void *data) {
	set_solitaire(event, solitaire_noname1);
}

static void sol_pyramid_callback(render_event_args *event, void *data) {
}

void render_object_mainmenu(render_object *parent) {
	render_object *window, *idiot, *malt, *noname1;
	RsvgHandle *h;
	float bw = 200.0f, bh = 64.0f;

	h = render_svg_open("resources/images/mainmenu.svg");

	window = widget_generic(render_object_mainmenu_id);
	widget_style_set_pos(window, 100.0f, 100.0f);
	widget_style_set_size(window, 500.0f, 412.0f);
	widget_style_set_image(window, h, "#mainmenu", 512, 512);
	render_object_add_child(parent, window);

	idiot = widget_generic(0);
	widget_style_set_pos(idiot, 45.0f, 85.0f);
	widget_style_set_size(idiot, bw, bh);
	widget_style_set_click_callback(idiot, sol_theidiot_callback);
	widget_style_set_image(idiot, h, "#sol_idiot", 256, 64);
	render_object_add_child(window, idiot);

	malt = widget_generic(0);
	widget_style_set_pos(malt, 45.0f, 154.0f);
	widget_style_set_size(malt, bw, bh);
	widget_style_set_click_callback(malt, sol_malteser_callback);
	widget_style_set_image(malt, h, "#sol_maltesercross", 256, 64);
	render_object_add_child(window, malt);

	noname1 = widget_generic(0);
	widget_style_set_pos(noname1, 45.0f, 223.0f);
	widget_style_set_size(noname1, bw, bh);
	widget_style_set_click_callback(noname1, sol_noname1_callback);
	widget_style_set_image(noname1, h, "#sol_noname1", 256, 64);
	render_object_add_child(window, noname1);

	render_svg_close(h);
}