#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include "api/render_widget.h"
#include "api/resource.h"
#include "render_topmenu.h"

const char *render_object_topmenu_id = "topmenu";

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
	char file_buffer[PATH_MAX];
	render_object *menu, *options, *quit, *window;
	widget_style *style;
	RsvgHandle *h;

	resource_locate_file("resources/images/topmenu.svg", file_buffer, PATH_MAX);
	h = render_svg_open(file_buffer);

	window = widget_generic(render_object_topmenu_id);
	style = widget_get_default_style(window);
	widget_style_set_backcolor(style, 1.0f, 1.0f, 1.0f, 1.0f);
	widget_style_set_pos(style, 0.0f, 0.0f);
	widget_style_set_left(style, "viewport_width-208");
	widget_style_set_size(style, 64.0f * 3 + 4 * 4.0f, 72.0f);
	render_object_add_child(parent, window);

	menu = widget_generic(0);
	style = widget_get_default_style(menu);
	widget_style_set_pos(style, 4.0f, 4.0f);
	widget_style_set_size(style, 64.0f, 64.0f);
	widget_style_set_click_callback(style, topmenu_menu_callback);
	widget_style_set_image(style, h, "#menu", 64, 64);
	render_object_add_child(window, menu);

	options = widget_generic(0);
	style = widget_get_default_style(options);
	widget_style_set_pos(style, 72.0f, 4.0f);
	widget_style_set_size(style, 64.0f, 64.0f);
	widget_style_set_click_callback(style, topmenu_options_callback);
	widget_style_set_image(style, h, "#options", 64, 64);
	render_object_add_child(window, options);

	quit = widget_generic(0);
	style = widget_get_default_style(quit);
	widget_style_set_pos(style, 140.0f, 4.0f);
	widget_style_set_size(style, 64.0f, 64.0f);
	widget_style_set_click_callback(style, topmenu_quit_callback);
	widget_style_set_image(style, h, "#quit", 64, 64);
	render_object_add_child(window, quit);

	render_svg_close(h);
}
