#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "api/mygl.h"
#include "api/render_widget.h"
#include "client/ui.h"
#include "client/ui_button.h"
#include "client/ui_menu_background.h"
#include "client/ui_mainmenu.h"

const char *render_object_mainmenu_id = "mainmenu";
bool render_testing = false;

static void solitaire_callback(render_event_args *event, void *data) {
	ui_scene_solitaire_select(event->rcontext);
}

static void card_theme_callback(render_event_args *event, void *data) {
	ui_scene_card_themes(event->rcontext);
}

static void quit_callback(render_event_args *event, void *data) {
	/* TODO: Have a better shutdown of the application. */
	exit(0);
}

render_object *ui_mainmenu(void) {
	render_object *window = widget_generic(render_object_mainmenu_id);
	widget_style *style = widget_get_default_style(window);
	widget_style_set_left(style, "0");
	widget_style_set_top(style, "0");
	widget_style_set_width(style, "viewport_width");
	widget_style_set_height(style, "viewport_height");

	render_object_add_child(window, ui_menu_background());

	float button_top = 140.0f;
	render_object_add_child(window, ui_button(0, expression_const(30),
	                                          expression_const(button_top),
	                                          "Solitaire", solitaire_callback));
	button_top += 40;

	render_object_add_child(
		window, ui_button(0, expression_const(30), expression_const(button_top),
	                      "Card theme", card_theme_callback));
	button_top += 40;

	render_object_add_child(window, ui_button(0, expression_const(30),
	                                          expression_const(button_top),
	                                          "Quit", quit_callback));
	button_top += 40;

	return window;
}
