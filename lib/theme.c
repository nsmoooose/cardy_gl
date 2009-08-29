#include <cairo/cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <glib.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "theme.h"

const int card_texture_width = 128;
const int card_texture_height = 256;

static char* g_card_suit_names[] = {"club", "diamond", "heart", "spade"};
static char* g_card_value_names[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "jack", "queen", "king"};

void theme_add_theme(mem_context *context, themes *themes, char *theme_name) {
	char** old_names;
	int buffer_size = strlen(theme_name) + 1;
	char *buffer = mem_alloc(context, buffer_size);
	strncpy(buffer, theme_name, buffer_size);

	old_names  = themes->theme_names;
	themes->theme_names = mem_alloc(context, (themes->theme_count + 1) * sizeof(char*));
	themes->theme_count++;

	if(old_names) {
		memcpy(themes->theme_names, old_names, sizeof(char*) * (themes->theme_count - 1));
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

void theme_render_card_texture(RsvgHandle *h, GLuint texture, char *node_name) {
/*
  This can be some source of information on how to render a svg file
  into a bitmap. This is the svg -> bitmap conversion tool part of
  gnome.

  git clone git://git.gnome.org/librsvg

  This file contains information about how to render things into bitmaps:
    rsvg-convert.c
*/
    RsvgDimensionData dimensions;
	RsvgPositionData pos;
	cairo_matrix_t matrix;
	int stride;
	unsigned char* cairo_data;
	cairo_surface_t *cairo_surface;
	cairo_t *cr;
	double xzoom=1.0f, yzoom=1.0f;

	if(!rsvg_handle_has_sub(h, node_name)) {
		fprintf(stderr, "%s wasn't found within the svg document.\n", node_name);
		exit(0);
	}

	if(!rsvg_handle_get_dimensions_sub(h, &dimensions, node_name)) {
		fprintf(stderr, "Failed to obtain the card dimensions for: %s\n", node_name);
		exit(0);
	}

	rsvg_handle_get_position_sub(h, &pos, node_name);
	if (dimensions.width <= 0 || dimensions.height <= 0) {
		exit(0);
	}
	stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, card_texture_width);
	cairo_data = (unsigned char *) calloc(stride * card_texture_height, 1);
	cairo_surface = cairo_image_surface_create_for_data(
		cairo_data, CAIRO_FORMAT_ARGB32, card_texture_width,
		card_texture_height, stride);

	cr = cairo_create(cairo_surface);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_rectangle (cr, 0, 0, card_texture_width, card_texture_height);
	cairo_fill (cr);

	xzoom = (double)card_texture_width / (double)dimensions.width;
	yzoom = (double)card_texture_height / (double)dimensions.height;

	cairo_matrix_init_identity(&matrix);
	cairo_matrix_scale(&matrix, xzoom, yzoom);
	cairo_matrix_translate(&matrix, 0 - pos.x, 0 - pos.y);

	cairo_set_matrix(cr, &matrix);

	if(!rsvg_handle_render_cairo_sub(h, cr, node_name)) {
		fprintf(stderr, "Failed to render image: %s.\n", node_name);
		exit(0);
	}

	/*
	char filename_buffer[24];
	snprintf(filename_buffer, sizeof(filename_buffer), "tmp/%s.png", node_name);
	cairo_surface_write_to_png (cairo_surface, filename_buffer);
	*/

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, card_texture_width, card_texture_height,
				 0, GL_BGRA, GL_UNSIGNED_BYTE, cairo_data);

	free(cairo_data);
	cairo_destroy(cr);
	cairo_surface_destroy(cairo_surface);
}

void theme_render_card_textures(theme *theme) {
	GError* e = NULL;
	RsvgHandle* h;
	card_suit suit;
	card_value value;
	char file_buffer[1000];
	char name_buffer[20];

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(53, theme->card_textures);

	snprintf(file_buffer, 1000, "%s/%s", theme->theme_directory, "cards.svg");
	h = rsvg_handle_new_from_file(file_buffer, &e);
	if(e != NULL) {
		exit(1);
	}

	for(suit=e_suit_first;suit<=e_suit_last;++suit) {
		for(value=1;value<=13;++value) {
			snprintf(name_buffer, sizeof(name_buffer), "#%s_%s", g_card_value_names[value-1], g_card_suit_names[suit]);
			theme_render_card_texture(h, theme_get_card_texture(theme, suit, value), name_buffer);
		}
	}
	theme_render_card_texture(h, theme_get_card_back_texture(theme), "#back");
	check_gl_errors("setup_render_resources");
}
