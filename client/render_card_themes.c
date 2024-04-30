#include "api/memory.h"
#include "api/render_widget.h"
#include "api/resource.h"
#include "api/theme.h"
#include "client/card_theme.h"
#include "client/render_card_themes.h"
#include "client/ui.h"
#include "client/ui_button.h"
#include "client/ui_menu_background.h"

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

	render_object_add_child(window, ui_menu_background());

	float button_top = 140.0f;
	char pos_top[20];

	mem_context *mem = mem_context_create();
	strncat(theme_dir, "resources/card_themes", PATH_MAX - 1);
	themes *themes = theme_list(mem, theme_dir);

	for (int i = 0; i < themes->theme_count; i++) {
		snprintf(pos_top, 20, "%f", button_top);
		render_object_add_child(
			window, ui_button(themes->theme_names[i], "30", pos_top,
		                      themes->theme_names[i], card_theme_callback));
		button_top += 40;
	}

	snprintf(pos_top, 20, "%f", button_top);
	render_object_add_child(window,
	                        ui_button(0, "30", pos_top, "Back", back_callback));
	button_top += 40;

	mem_context_free(mem);
}
