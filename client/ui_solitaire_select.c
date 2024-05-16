#include "api/solitaires/solitaires.h"
#include "client/ui.h"
#include "client/ui_button.h"
#include "client/ui_mainmenu.h"
#include "client/ui_menu_background.h"
#include "client/ui_solitaire_select.h"

/* Card settings */
static int cs_top = 140;
static int cs_left = 40;
static int cs_card_height = 300;
static int cs_card_separation = 20;
static int cs_items_per_row = 4;
static int cs_card_width = 200;

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

static render_object *sol_card_create(const char *key, game *game, int index) {
	render_object *card = widget_generic(0);
	widget_style *style = widget_get_default_style(card);

	int row = index / cs_items_per_row;
	int row_index = index % cs_items_per_row;
	char tmp[40];
	snprintf(tmp, 40, "40+%d*%d", row_index, cs_card_width);
	widget_style_set_left_e(
		style, expression_const(
				   cs_left + row_index * (cs_card_width + cs_card_separation)));
	widget_style_set_top_e(
		style,
		expression_const(cs_top + row * (cs_card_height + cs_card_separation)));
	widget_style_set_width(style, "200");
	widget_style_set_height(style, "300");
	widget_style_set_backcolor(style, 0.7, 0.7, 0.7, 0.9);

	render_object *button =
		ui_button(key, expression_const(10), expression_const(270), game->name,
	              sol_callback);
	render_object_add_child(card, button);
	return card;
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
	int index = 0;
	while (g_hash_table_iter_next(&it, &key, &value)) {
		game *game = value;
		if (!render_testing && game->testing == true) {
			continue;
		}

		render_object_add_child(window, sol_card_create(key, game, index));
		button_top += 40;
		index++;
	}
	game_registry_free(registry);

	render_object_add_child(
		window, ui_button(key,
	                      expression_const(cs_left + cs_items_per_row / 2 *
	                                                     (cs_card_width +
	                                                      cs_card_separation)),
	                      expression_const(cs_top + 2 * (cs_card_height +
	                                                     cs_card_separation)),
	                      "Back", back_callback));
	button_top += 40;

	return window;
}
