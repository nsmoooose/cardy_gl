#include "api/memory.h"
#include "api/render_widget.h"
#include "api/resource.h"
#include "api/theme.h"
#include "game/card_theme.h"
#include "game/render_card_themes.h"
#include "game/ui.h"

const char *render_object_card_themes_id = "mainmenu";

static void card_theme_callback(render_event_args *event, void *data) {
	card_theme_set(event->object->id);
	ui_scene_main(event->rcontext);
}

static void back_callback(render_event_args *event, void *data) {
	ui_scene_main(event->rcontext);
}

void render_object_card_themes(render_object *parent) {
	char theme_dir[PATH_MAX];
	if (!resource_get_dir(theme_dir, PATH_MAX)) {
		exit(1);
	}

	render_object *window = widget_generic(render_object_card_themes_id);
	widget_style *style = widget_get_default_style(window);
	widget_style_set_left(style, "0");
	widget_style_set_top(style, "0");
	widget_style_set_width(style, "viewport_width");
	widget_style_set_height(style, "viewport_height");
	render_object_add_child(parent, window);

	float button_top = 140.0f;

	mem_context *mem = mem_context_create();
	strncat(theme_dir, "resources/card_decks/french/themes", PATH_MAX - 1);
	themes *themes = theme_list(mem, theme_dir);

	for (int i = 0; i < themes->theme_count; i++) {
		render_object *button = widget_generic(themes->theme_names[i]);
		style = widget_get_default_style(button);
		widget_style_set_image_size(style, 128, 32);
		/* widget_style_set_font_face(style, "Meera"); */
		widget_style_set_font_size(style, 16.0f);
		widget_style_set_text(style, themes->theme_names[i]);
		widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 0.0f);
		widget_style_set_size(style, 128.0f, 32.0f);
		widget_style_set_pos(style, 30.0f, button_top);
		widget_style_set_click_callback(style, card_theme_callback);
		render_object_add_child(window, button);

		button_top += 40;
	}

	render_object *button = widget_generic(0);
	style = widget_get_default_style(button);
	widget_style_set_image_size(style, 128, 32);
	/* widget_style_set_font_face(style, "Meera"); */
	widget_style_set_font_size(style, 16.0f);
	widget_style_set_text(style, "Back");
	widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 0.0f);
	widget_style_set_size(style, 128.0f, 32.0f);
	widget_style_set_pos(style, 30.0f, button_top);
	widget_style_set_click_callback(style, back_callback);
	render_object_add_child(window, button);
	button_top += 40;

	mem_context_free(mem);
}
