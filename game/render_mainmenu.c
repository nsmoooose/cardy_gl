#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include "../api/mygl.h"
#include "../api/render_widget.h"
#include "../api/resource.h"
#include "render_solitaire.h"
#include "render_solved.h"
#include "render_mainmenu.h"
#include "solitaire_heirship.h"
#include "solitaire_theidiot.h"
#include "solitaire_maltesercross.h"
#include "solitaire_noname1.h"
#include "solitaire_pyramid.h"
#include "solitaire_test1.h"

const char* render_object_mainmenu_id = "mainmenu";

static void set_solitaire(render_event_args *event, solitaire_create sol_callback) {
	render_object *placeholder = render_object_find(
		event->rcontext->object, "placeholder");
	render_object *existing_sol = render_object_find(
		event->rcontext->object, render_object_solitaire_id);
	render_object *solved = render_object_find(
		event->rcontext->object, render_object_solved_id);

	if(existing_sol) {
		render_object_free(event->rcontext, existing_sol);
	}
	if(solved) {
		render_object_free(event->rcontext, solved);
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
	set_solitaire(event, solitaire_pyramid);
}

static void sol_test1_callback(render_event_args *event, void *data) {
	set_solitaire(event, solitaire_test1);
}

static void sol_heirship_callback(render_event_args *event, void *data) {
	set_solitaire(event, solitaire_heirship);
}

void render_object_mainmenu(render_object *parent) {
	char file_buffer[PATH_MAX];
	render_object *window, *button;
	widget_style *style;
	RsvgHandle *h;
	float bw = 200.0f, bh = 64.0f, button_top=40.0f;

	resource_locate_file("resources/images/mainmenu.svg", file_buffer, PATH_MAX);
	h = render_svg_open(file_buffer);

	window = widget_generic(render_object_mainmenu_id);
	style = widget_get_default_style(window);
	widget_style_set_left(style, "0");
	widget_style_set_top(style, "0");
	widget_style_set_width(style, "viewport_width");
	widget_style_set_height(style, "viewport_height");
	render_object_add_child(parent, window);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_left(style, "viewport_width/2-width/2");
	widget_style_set_top(style, "viewport_height/2-height/2");
	widget_style_set_size(style, 300.0f, 300.0f);
	widget_style_set_image(style, h, "#background", 512, 512);
	/* widget_style_set_rotation(style, "45"); */
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 0.0f, 0.0f);
	widget_style_set_size(style, 342.0f, 138.0f);
	widget_style_set_image(style, h, "#logo", 256, 128);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 45.0f, 85.0f + button_top);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_theidiot_callback);
	widget_style_set_image(style, h, "#sol_idiot", 256, 64);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 45.0f, 154.0f + button_top);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_malteser_callback);
	widget_style_set_image(style, h, "#sol_maltesercross", 256, 64);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 45.0f, 223.0f + button_top);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_noname1_callback);
	widget_style_set_image(style, h, "#sol_noname1", 256, 64);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 45.0f, 292.0f + button_top);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_pyramid_callback);
	widget_style_set_image(style, h, "#sol_pyramid", 256, 64);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 45.0f, 361.0f + button_top);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_test1_callback);
	widget_style_set_image(style, h, "#sol_test1", 256, 64);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 45.0f, 430.0f + button_top);
	widget_style_set_size(style, bw, bh);
	widget_style_set_click_callback(style, sol_heirship_callback);
	widget_style_set_image(style, h, "#sol_heirship", 256, 64);
	render_object_add_child(window, button);

	render_svg_close(h);
}
