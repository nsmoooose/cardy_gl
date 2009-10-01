#include <librsvg/rsvg.h>
#include <stdlib.h>
#include "../api/render_widget.h"
#include "render_topmenu.h"

const char* render_object_topmenu_id = "topmenu";

static void topmenu_menu_callback(render_event_args *event, void *data) {
	printf("Go to main menu.\n");
}

static void topmenu_options_callback(render_event_args *event, void *data) {
	printf("Go to options\n");
}

static void topmenu_quit_callback(render_event_args *event, void *data) {
	exit(0);
}

void render_object_topmenu(render_object *parent) {
	render_object *menu, *options, *quit, *window;
	RsvgHandle *h;

	h = render_svg_open("resources/images/topmenu.svg");

	window = widget_generic(render_object_topmenu_id);
	widget_style_set_backcolor(window, 1.0f, 1.0f, 1.0f, 1.0f);
	widget_style_set_pos(window, 0.0f, 0.0f);
	widget_style_set_size(window, 64.0f * 3 + 4 * 4.0f, 72.0f);
	render_object_add_child(parent, window);

	menu = widget_generic(0);
	widget_style_set_pos(menu, 4.0f, 4.0f);
	widget_style_set_size(menu, 64.0f, 64.0f);
	widget_style_set_click_callback(menu, topmenu_menu_callback);
	widget_style_set_image(menu, h, "#menu", 64, 64);
	render_object_add_child(window, menu);

	options = widget_generic(0);
	widget_style_set_pos(options, 72.0f, 4.0f);
	widget_style_set_size(options, 64.0f, 64.0f);
	widget_style_set_click_callback(options, topmenu_options_callback);
	widget_style_set_image(options, h, "#options", 64, 64);
	render_object_add_child(window, options);

	quit = widget_generic(0);
	widget_style_set_pos(quit, 140.0f, 4.0f);
	widget_style_set_size(quit, 64.0f, 64.0f);
	widget_style_set_click_callback(quit, topmenu_quit_callback);
	widget_style_set_image(quit, h, "#quit", 64, 64);
	render_object_add_child(window, quit);

	render_svg_close(h);
}
