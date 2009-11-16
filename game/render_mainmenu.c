#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include "../api/mygl.h"
#include "../api/render_widget.h"
#include "../api/resource.h"
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

/*
static void sol_pyramid_callback(render_event_args *event, void *data) {
}
*/

void render_object_mainmenu(render_object *parent) {
	char file_buffer[PATH_MAX];
	render_object *window, *idiot, *malt, *noname1;
	widget_style *style;
	RsvgHandle *h;
	float bw = 200.0f, bh = 64.0f;

	resource_locate_file("resources/images/mainmenu.svg", file_buffer, PATH_MAX);
	h = render_svg_open(file_buffer);

	window = widget_generic(render_object_mainmenu_id);
	style = widget_get_default_style(window);
	widget_style_set_left(style, "viewport_width/2-width/2");
	widget_style_set_top(style, "viewport_height/2-height/2");
	widget_style_set_size(style, 500.0f, 412.0f);
	widget_style_set_image(style, h, "#mainmenu", 512, 512);
	render_object_add_child(parent, window);

	idiot = widget_generic(0);
	style = widget_get_default_style(idiot);
	widget_style_set_pos(style, 45.0f, 85.0f);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_theidiot_callback);
	widget_style_set_image(style, h, "#sol_idiot", 256, 64);
	render_object_add_child(window, idiot);

	malt = widget_generic(0);
	style = widget_get_default_style(malt);
	widget_style_set_pos(style, 45.0f, 154.0f);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_malteser_callback);
	widget_style_set_image(style, h, "#sol_maltesercross", 256, 64);
	render_object_add_child(window, malt);

	noname1 = widget_generic(0);
	style = widget_get_default_style(noname1);
	widget_style_set_pos(style, 45.0f, 223.0f);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_noname1_callback);
	widget_style_set_image(style, h, "#sol_noname1", 256, 64);
	render_object_add_child(window, noname1);

	render_svg_close(h);
}
