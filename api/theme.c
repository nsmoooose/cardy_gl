#include <cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <glib.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "render.h"
#include "theme.h"

const int card_texture_width = 128;
const int card_texture_height = 256;

static char* g_card_suit_names[] = {
	"club", "diamond", "heart", "spade"
};

static char* g_card_value_names[] = {
	"1", "2", "3", "4", "5", "6", "7",
	"8", "9", "10", "jack", "queen", "king"
};

void theme_add_theme(mem_context *context, themes *themes, char *theme_name) {
	char** old_names;
	int buffer_size = strlen(theme_name) + 1;
	char *buffer = mem_alloc(context, buffer_size);
	strncpy(buffer, theme_name, buffer_size);

	old_names  = themes->theme_names;
	themes->theme_names =
		mem_alloc(context, (themes->theme_count + 1) * sizeof(char*));
	themes->theme_count++;

	if(old_names) {
		memcpy(themes->theme_names, old_names, sizeof(char*) *
			   (themes->theme_count - 1));
		mem_free(context, old_names);
	}
	themes->theme_names[themes->theme_count - 1] = buffer;
}

themes *theme_list(mem_context *context, char *themes_directory) {
	struct dirent *dp;
	themes *t;
	DIR *dir = opendir(themes_directory);
	if(dir == 0) {
		return 0;
	}

	t = mem_alloc(context, sizeof(themes));
	while ((dp=readdir(dir)) != 0) {
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
			continue;
		}
		theme_add_theme(context, t, dp->d_name);
	}
	closedir(dir);
	return t;
}

theme *theme_load(char *themes_directory, char *name) {
	theme *t;
	char *buffer = calloc(1, 1000);
	snprintf(buffer, 1000, "%s/%s", themes_directory, name);

	t = calloc(1, sizeof(theme));
	t->theme_directory = buffer;
	theme_render_card_textures(t);
	return t;
}

void theme_unload(theme *theme) {
	free(theme->theme_directory);
	glDeleteTextures(53, theme->card_textures);
	free(theme);
}

GLuint theme_get_card_texture(theme *theme, card_suit suit, card_value value) {
	if(value == 14) {
		return theme->card_textures[suit * 13 + 1];
	}
	else {
		return theme->card_textures[suit * 13 + value];
	}
}

GLuint theme_get_card_back_texture(theme *theme) {
	return theme->card_textures[0];
}

void theme_render_card_textures(theme *theme) {
	RsvgHandle* h;
	card_suit suit;
	card_value value;
	char file_buffer[1000];
	char name_buffer[20];
	GLuint card_texture;

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(53, theme->card_textures);

	snprintf(file_buffer, 1000, "%s/%s", theme->theme_directory, "cards.svg");
	h = render_svg_open(file_buffer);
	if(h == 0) {
		exit(1);
	}

	for(suit=e_suit_first;suit<=e_suit_last;++suit) {
		for(value=1;value<=13;++value) {
			snprintf(name_buffer, sizeof(name_buffer),
					 "#%s_%s", g_card_value_names[value-1],
					 g_card_suit_names[suit]);
			card_texture = theme_get_card_texture(theme, suit, value);
			render_svg_texture(h, card_texture, name_buffer,
							  card_texture_width, card_texture_height);
		}
	}
	render_svg_texture(h, theme_get_card_back_texture(theme), "#back",
					   card_texture_width, card_texture_height);
	render_svg_close(h);
	check_gl_errors("setup_render_resources");
}
