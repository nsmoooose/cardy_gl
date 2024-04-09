#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include "api/render_widget.h"
#include "api/resource.h"
#include "client/render_topmenu.h"
#include "client/ui.h"

const char *render_object_topmenu_id = "topmenu";

static void topmenu_menu_callback(render_event_args *event, void *data) {
	ui_scene_main(event->rcontext);
}

static void topmenu_quit_callback(render_event_args *event, void *data) {
	exit(0);
}

void render_object_topmenu(render_object *parent) {
	char file_buffer[PATH_MAX];
	widget_style *style;
	RsvgHandle *h;

	resource_locate_file("resources/images/topmenu.svg", file_buffer, PATH_MAX);
	h = render_svg_open(file_buffer);

	render_object *window = widget_generic(render_object_topmenu_id);
	style = widget_get_default_style(window);
	widget_style_set_backcolor(style, 1.0f, 1.0f, 1.0f, 1.0f);
	widget_style_set_pos(style, 0.0f, 0.0f);
	widget_style_set_size(style, 64.0f * 2 + 4 * 4.0f, 72.0f);
	render_object_add_child(parent, window);

	render_object *menu = widget_generic(0);
	style = widget_get_default_style(menu);
	widget_style_set_pos(style, 4.0f, 4.0f);
	widget_style_set_size(style, 64.0f, 64.0f);
	widget_style_set_click_callback(style, topmenu_menu_callback);
	widget_style_set_image(style, h, "#menu", 64, 64);
	render_object_add_child(window, menu);

	render_object *quit = widget_generic(0);
	style = widget_get_default_style(quit);
	widget_style_set_pos(style, 72.0f, 4.0f);
	widget_style_set_size(style, 64.0f, 64.0f);
	widget_style_set_click_callback(style, topmenu_quit_callback);
	widget_style_set_image(style, h, "#quit", 64, 64);
	render_object_add_child(window, quit);

	render_svg_close(h);
}
