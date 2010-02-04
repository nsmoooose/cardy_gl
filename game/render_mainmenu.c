#include <librsvg/rsvg.h>
#include <limits.h>
#include <stdlib.h>
#include "../api/mygl.h"
#include "../api/render_widget.h"
#include "../api/resource.h"
#include "../api/solitaires/solitaires.h"
#include "render_solitaire.h"
#include "render_solved.h"
#include "render_mainmenu.h"

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

static void sol_callback(render_event_args *event, void *data) {
	game_registry *registry = solitaire_get_registry();
	game *game = g_hash_table_lookup(registry->games, event->object->id);
	if(game) {
		set_solitaire(event, game->create_instance);
	}
	game_registry_free(registry);
}

static void quit_callback(render_event_args *event, void *data) {
	/* TODO: Have a better shutdown of the application. */
	exit(0);
}

void render_object_mainmenu(render_object *parent) {
	char file_buffer[PATH_MAX];
	render_object *window, *button;
	widget_style *style;
	RsvgHandle *h;
	float button_top=140.0f;
	GHashTableIter it;
	gpointer key, value;
	game_registry *registry = solitaire_get_registry();

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
	widget_style_set_rotation(style, style_key_render_time);
	render_object_add_child(window, button);

	button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_pos(style, 0.0f, 0.0f);
	widget_style_set_size(style, 342.0f, 138.0f);
	widget_style_set_image(style, h, "#logo", 256, 128);
	render_object_add_child(window, button);
	render_svg_close(h);

	g_hash_table_iter_init(&it, registry->games);
	while(g_hash_table_iter_next(&it, &key, &value)) {
		game *game = value;
		button = widget_generic(key);
		style = widget_get_default_style(button);
		widget_style_set_image_size(style, 128, 32);
		/* widget_style_set_font_face(style, "Meera"); */
		widget_style_set_font_size(style, 16.0f);
		widget_style_set_text(style, game->name);
		widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 0.0f);
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
	widget_style_set_text(style, "Quit");
	widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 0.0f);
	widget_style_set_size(style, 128.0f, 32.0f);
	widget_style_set_pos(style, 30.0f, button_top);
	widget_style_set_click_callback(style, quit_callback);
	render_object_add_child(window, button);
	button_top += 40;
}
