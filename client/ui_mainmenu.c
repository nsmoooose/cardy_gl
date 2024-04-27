#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "api/mygl.h"
#include "api/render_widget.h"
#include "api/solitaires/solitaires.h"
#include "client/ui.h"
#include "client/ui_menu_background.h"
#include "client/ui_mainmenu.h"

const char *render_object_mainmenu_id = "mainmenu";
bool render_testing = false;

static void sol_callback(render_event_args *event, void *data) {
	game_registry *registry = solitaire_get_registry();
	game *game = g_hash_table_lookup(registry->games, event->object->id);
	if (game) {
		ui_scene_solitaire(event->rcontext, game->create_instance);
	}
	game_registry_free(registry);
}

static void card_theme_callback(render_event_args *event, void *data) {
	ui_scene_card_themes(event->rcontext);
}

static void quit_callback(render_event_args *event, void *data) {
	/* TODO: Have a better shutdown of the application. */
	exit(0);
}

render_object *ui_mainmenu(void) {
	render_object *window, *button;
	widget_style *style;
	float button_top = 140.0f;
	GHashTableIter it;
	gpointer key, value;
	game_registry *registry = solitaire_get_registry();

	window = widget_generic(render_object_mainmenu_id);
	style = widget_get_default_style(window);
	widget_style_set_left(style, "0");
	widget_style_set_top(style, "0");
	widget_style_set_width(style, "viewport_width");
	widget_style_set_height(style, "viewport_height");

	render_object_add_child(window, ui_menu_background());

	g_hash_table_iter_init(&it, registry->games);
	while (g_hash_table_iter_next(&it, &key, &value)) {
		game *game = value;
		if (!render_testing && game->testing == true) {
			continue;
		}
		button = widget_generic(key);
		style = widget_get_default_style(button);
		widget_style_set_image_size(style, 128, 32);
		/* widget_style_set_font_face(style, "Meera"); */
		widget_style_set_font_size(style, 16.0f);
		widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 1.0f);
		widget_style_set_text(style, game->name);
		widget_style_set_size(style, 128.0f, 32.0f);
		widget_style_set_pos(style, 30.0f, button_top);
		widget_style_set_click_callback(style, sol_callback);
		render_object_add_child(window, button);

		button_top += 40;
	}
	game_registry_free(registry);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_image_size(style, 128, 32);
	/* widget_style_set_font_face(style, "Meera"); */
	widget_style_set_font_size(style, 16.0f);
	widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 1.0f);
	widget_style_set_text(style, "Card theme");
	widget_style_set_size(style, 128.0f, 32.0f);
	widget_style_set_pos(style, 30.0f, button_top);
	widget_style_set_click_callback(style, card_theme_callback);
	render_object_add_child(window, button);
	button_top += 40;

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_image_size(style, 128, 32);
	/* widget_style_set_font_face(style, "Meera"); */
	widget_style_set_font_size(style, 16.0f);
	widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 1.0f);
	widget_style_set_text(style, "Quit");
	widget_style_set_size(style, 128.0f, 32.0f);
	widget_style_set_pos(style, 30.0f, button_top);
	widget_style_set_click_callback(style, quit_callback);
	render_object_add_child(window, button);
	button_top += 40;

	return window;
}
