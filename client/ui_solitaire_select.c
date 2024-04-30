#include "api/solitaires/solitaires.h"
#include "client/ui.h"
#include "client/ui_button.h"
#include "client/ui_mainmenu.h"
#include "client/ui_menu_background.h"
#include "client/ui_solitaire_select.h"

static void sol_callback(render_event_args *event, void *data) {
	game_registry *registry = solitaire_get_registry();
	game *game = g_hash_table_lookup(registry->games, event->object->id);
	if (game) {
		ui_scene_solitaire(event->rcontext, game->create_instance);
	}
	game_registry_free(registry);
}

static void back_callback(render_event_args *event, void *data) {
	ui_scene_main(event->rcontext);
}

render_object *ui_solitaire_select(void) {
	game_registry *registry = solitaire_get_registry();

	render_object *window = widget_generic(0);
	widget_style *style = widget_get_default_style(window);
	widget_style_set_left(style, "0");
	widget_style_set_top(style, "0");
	widget_style_set_width(style, "viewport_width");
	widget_style_set_height(style, "viewport_height");

	render_object_add_child(window, ui_menu_background());

	GHashTableIter it;
	gpointer key, value;
	g_hash_table_iter_init(&it, registry->games);
	float button_top = 140.0f;
	while (g_hash_table_iter_next(&it, &key, &value)) {
		game *game = value;
		if (!render_testing && game->testing == true) {
			continue;
		}

		render_object_add_child(window, ui_button(key, expression_const(30),
		                                          expression_const(button_top),
		                                          game->name, sol_callback));
		button_top += 40;
	}
	game_registry_free(registry);

	render_object_add_child(window, ui_button(key, expression_const(30),
	                                          expression_const(button_top),
	                                          "Back", back_callback));
	button_top += 40;

	return window;
}
