#include "api/resource.h"
#include "game/card_theme.h"

static theme *g_theme = NULL;

theme *card_theme_get(void) {
	if (!g_theme) {
		card_theme_set("ornamental");
	}
	return g_theme;
}

void card_theme_set(const char *name) {
	if (g_theme) {
		theme_unload(g_theme);
		g_theme = NULL;
	}

	char theme_dir[PATH_MAX];
	if (resource_get_dir(theme_dir, PATH_MAX)) {
		strncat(theme_dir, "resources/card_decks/french/themes", PATH_MAX - 1);
		g_theme = theme_load(theme_dir, name);
	}
}
