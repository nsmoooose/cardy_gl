#include <cairo/cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/image.h"
#include "lib/mygl.h"
#include "rendering.h"

/** Global instance of the one and only solitaire that is currently
 *  running.
 */
solitaire* g_solitaire = 0;
mem_context *g_context = 0;

float g_camera_zoom = -500.0f;
float g_camera_translateX = 0.0f;
float g_camera_translateY = 0.0f;
float g_perspective_near = 1.0f;
float g_perspective_far = 5000.0f;
float g_perspective_fov = 45.0f;

card_proxy *g_selected_card = 0;

/* Build a vector of coordinates for a card. */
static GLfloat g_card_vertexes[8*3] = {
	0 - CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 0, top left, front */
	0 + CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 1, top right, front */
	0 + CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 2, bottom right, front */
	0 - CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 3, bottom left, front */
	0 - CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f, /* 4, top left, back */
	0 + CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f, /* 5, top right, back */
	0 + CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f, /* 6, bottom right, back */
	0 - CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f /* 7, bottom left, back */
};

static GLubyte g_card_indexes[] = {
	0, 1, 2, 3,
	4, 5, 1, 0,
	3, 2, 6, 7,
	5, 4, 7, 6,
	1, 5, 6, 2,
	4, 0, 3, 7
};

/* Card textures. First one is the back of a card. Index 1-52 are the actual cards. */
static GLuint g_card_textures[53] = {
	0,
	0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0,
	0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0,
	0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0,
	0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0
};

static char* g_card_suit_names[] = {"diamond", "club", "heart", "spade"};
static char* g_card_value_names[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "jack", "queen", "king"};

char *get_card_suit_name(card_suit suit) {
	return g_card_suit_names[suit];
}

char *get_card_value_name(card_value value) {
	return g_card_value_names[value-1];
}

GLuint get_card_texture(card *card) {
	return g_card_textures[card->suit * 13 + card->value];
}

GLuint get_card_back_texture() {
	return g_card_textures[0];
}

void setup_render_resources() {
	GError* e = NULL;
	RsvgHandle* h;
    RsvgDimensionData dimensions;
	RsvgPositionData pos;
	cairo_matrix_t matrix;
	int stride;
	unsigned char* cairo_data;
	cairo_surface_t *cairo_surface;
	cairo_t *cr;

	card card;
	card_suit suit;
	card_value value;
	char name_buffer[20];
	char filename_buffer[24];

/*
  This can be some source of information on how to render a svg file
  into a bitmap. This is the svg -> bitmap conversion tool part of
  gnome.

  git clone git://git.gnome.org/librsvg

  This file contains information about how to render things into bitmaps:
    rsvg-convert.c
*/
/*
	GLbyte *image;
	GLint width, height, components;
	GLenum format;
*/
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures(1, g_card_textures);

	rsvg_init();
	h = rsvg_handle_new_from_file("images/gnomangelo_bitmap.svg", &e);
	if(e != NULL) {
		exit(1);
	}

	for(suit=e_suit_first;suit<e_suit_last;++suit) {
		card.suit = suit;
		for(value=1;value<=13;++value) {
			card.value = value;

			snprintf(name_buffer, sizeof(name_buffer), "#%s_%s", get_card_value_name(value), get_card_suit_name(suit));

			if(!rsvg_handle_has_sub(h, name_buffer)) {
				fprintf(stderr, "%s wasn't found within the svg document.\n", name_buffer);
				continue;
			}

			if(!rsvg_handle_get_dimensions_sub(h, &dimensions, name_buffer)) {
				fprintf(stderr, "Failed to obtain the card dimensions for: %s\n", name_buffer);
				exit(0);
			}

			rsvg_handle_get_position_sub(h, &pos, name_buffer);
			if (dimensions.width <= 0 || dimensions.height <= 0) {
				exit(0);
			}
			printf("Width: %d, Height: %d, Position: (%d,%d)\n",
				   dimensions.width, dimensions.height, pos.x, pos.y);

			stride = dimensions.width * 4;

			cairo_data = (unsigned char *) calloc(stride * dimensions.height, 1);
			cairo_surface = cairo_image_surface_create_for_data(
				cairo_data, CAIRO_FORMAT_ARGB32, dimensions.width,
				dimensions.height, stride);

			cr = cairo_create(cairo_surface);
			cairo_set_source_rgb (cr, 0.0, 1.0, 1.0);
			cairo_rectangle (cr, 0, 0, dimensions.width, dimensions.height);
			cairo_fill (cr);

			cairo_matrix_init_identity(&matrix);
			/* cairo_matrix_scale(&matrix, xzoom, yzoom); */
			/* cairo_matrix_translate(&matrix, pos.x, pos.y); */
			cairo_matrix_translate(&matrix, 0 - pos.x, 0 - pos.y);

			cairo_set_matrix(cr, &matrix);

			if(!rsvg_handle_render_cairo_sub(h, cr, name_buffer)) {
				fprintf(stderr, "Failed to render image: %s.\n", name_buffer);
			}

			snprintf(filename_buffer, sizeof(filename_buffer), "tmp/%s.png", name_buffer);
			cairo_surface_write_to_png (cairo_surface, filename_buffer);

			free(cairo_data);
			cairo_destroy(cr);
			cairo_surface_destroy(cairo_surface);
/*
			glBindTexture(GL_TEXTURE_2D, get_card_texture(&card));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, dimensions.width, dimensions.height,
						 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, cairo_data);
*/
		}
	}

	exit(0);
}

void update_camera_pos() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(g_camera_translateX, g_camera_translateY, g_camera_zoom);
}

void render_solitaire(solitaire* sol) {
	int pile_index;
	int pile_count = sol->visual->pile_count;
	for(pile_index=0;pile_index<pile_count;++pile_index) {
		visual_pile* pile = sol->visual->piles[pile_index];
		if(!pile) {
			continue;
		}
		render_pile(pile);
	}
}

void render_pile(visual_pile* pile) {
	int card_index;

	/* Do a translation of our position for the pile. */
	glPushMatrix();
	glTranslatef(pile->origin[0], pile->origin[1], pile->origin[2]);

	glLoadName((GLuint)pile);

	if(pile->rotation != 0.0f) {
		glRotatef(pile->rotation, 0.0f, 0.0f, 1.0f);
	}

	glColor3f(0.0f, 0.7f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(0 - CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f);
	glVertex2f(0 + CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f);
	glVertex2f(0 + CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f);
	glVertex2f(0 - CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f);
	glEnd();

	glPushMatrix();
	for(card_index=0;card_index<pile->card_count;++card_index) {
		render_card(pile, pile->cards[card_index]);
	}
	glPopMatrix();

	glPopMatrix();
}

void render_card(visual_pile* pile, card_proxy* proxy) {
	int index;

	if(g_selected_card == proxy) {
		glColor3f(1.0f, 0.7f, 0.7f);
	}
	else {
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	glPushName((GLuint)proxy);

	/* Test to see if we need to rotate the card around its axis
	   to show the front face. */
	glPushMatrix();
	if(proxy->card != 0) {
		/* Rotate the card to show the front face of the card. */
	}

	for(index=0;index<6;++index) {
		/* Only apply texturing on the front and the back of the
		   card. */
		if(index == 0 && proxy->card != 0) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, get_card_texture(proxy->card));
		}
		if(index == 3) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, get_card_back_texture());
		}

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+0] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+0] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+0] * 3 + 2]);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+1] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+1] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+1] * 3 + 2]);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+2] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+2] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+2] * 3 + 2]);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+3] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+3] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+3] * 3 + 2]);
		glEnd();

		if(index == 0 || index == 3) {
			glDisable(GL_TEXTURE_2D);
		}
	}

	glPopMatrix();
	glPopName();

	/* Do a translation of our position for the next card. */
	glTranslatef(pile->translateX, pile->translateY, CARD_THICKNESS);
}

void render_desktop() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void render_scene() {
	GLenum errCode;
	const GLubyte *errString;

	glInitNames();
	glPushName(0);

	render_desktop();
	render_solitaire(g_solitaire);

	/* Print errors if there are any. */
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf(stderr, "OpenGL Error: %s\n", errString);
	}

	glutSwapBuffers();
}
