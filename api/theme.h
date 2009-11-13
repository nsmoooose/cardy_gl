#ifndef __THEME_H__
#define __THEME_H__

#include <librsvg/rsvg.h>
#include "card.h"
#include "mygl.h"

typedef struct {
	/** Card textures.
	 *  0 == back side
	 *  1 == ace
	 *  ...
	 *  52 == king
	 *  53 == joker 1
	 *  54 == joker 2
	 */
	GLuint card_textures[55];

	char *theme_directory;
} theme;

typedef struct {
	char **theme_names;
	int theme_count;
} themes;

themes *theme_list(mem_context *context, const char *themes_directory);
theme *theme_load(const char *themes_directory, const char *name);
void theme_unload(theme *theme);

GLuint theme_get_card_texture(theme *theme, card_suit suit, card_value value);
GLuint theme_get_card_back_texture(theme *theme);

void theme_render_card_texture(RsvgHandle *h, GLuint texture, char *node_name);
void theme_render_card_textures(theme *theme);

#endif /* __THEME_H__ */
