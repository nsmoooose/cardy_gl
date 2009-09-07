#include <cairo/cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "mygl.h"
#include "render_solitaire.h"
#include "theme.h"

/** Global instance of the one and only solitaire that is currently
 *  running.
 */
solitaire* g_solitaire = 0;
theme *g_theme = 0;

float g_camera_zoom = -500.0f;
float g_camera_translateX = 0.0f;
float g_camera_translateY = 0.0f;
float g_perspective_near = 1.0f;
float g_perspective_far = 5000.0f;
float g_perspective_fov = 45.0f;

card_proxy *g_selected_card = 0;
visual_settings *g_visual_settings = 0;

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
	0, 1, 2, 3, /* front face */
	4, 5, 1, 0, /* top */
	3, 2, 6, 7, /* bottom */
	5, 4, 7, 6, /* back face */
	1, 5, 6, 2, /* right */
	4, 0, 3, 7 /* left */
};

visual_settings *visual_settings_create(mem_context *context) {
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	settings->card_width = 40.0f;
	settings->card_height = 60.0f;
	settings->card_spacing = 4.0f;
	settings->card_thickness = 0.4f;
	return settings;
}

void render_update_camera_pos() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(g_camera_translateX, g_camera_translateY, g_camera_zoom);
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
	if(proxy->card == 0) {
		/* Rotate the card to show the front face of the card. */
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	}

	for(index=0;index<6;++index) {
		/* Only apply texturing on the front and the back of the
		   card. */
		if(index == 0 && proxy->card != 0) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, theme_get_card_texture(g_theme, proxy->card->suit, proxy->card->value));
		}
		if(index == 3) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, theme_get_card_back_texture(g_theme));
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
	check_gl_errors("render_card");
}

void render_pile(visual_pile* pile, visual_settings *settings) {
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
	glVertex2f(0 - settings->card_width/2.0f, 0 + settings->card_height/2.0f);
	glVertex2f(0 + settings->card_width/2.0f, 0 + settings->card_height/2.0f);
	glVertex2f(0 + settings->card_width/2.0f, 0 - settings->card_height/2.0f);
	glVertex2f(0 - settings->card_width/2.0f, 0 - settings->card_height/2.0f);
	glEnd();

	check_gl_errors("render_pile (1)");

	glPushMatrix();
	for(card_index=0;card_index<pile->card_count;++card_index) {
		render_card(pile, pile->cards[card_index]);
	}
	glPopMatrix();

	glPopMatrix();
	check_gl_errors("render_pile (2)");
}

void render_object_solitaire_render(render_object *object, float delta) {
	int pile_index;
	int pile_count = g_solitaire->visual->pile_count;
	for(pile_index=0;pile_index<pile_count;++pile_index) {
		visual_pile* pile = g_solitaire->visual->piles[pile_index];
		if(!pile) {
			continue;
		}
		render_pile(pile, g_solitaire->visual->settings);
	}
}

render_object *render_object_solitaire(mem_context *context) {
	render_object *o = render_object_create(context, "solitaire");
	o->render = render_object_solitaire_render;

	if(g_visual_settings == 0) {
		g_visual_settings = mem_alloc(context, sizeof(visual_settings));
		g_visual_settings->card_width = 40.0f;
		g_visual_settings->card_height = 60.0f;
		g_visual_settings->card_spacing = 4.0f;
		g_visual_settings->card_thickness = 0.4f;
	}
	if(g_theme == 0) {
		g_theme = theme_load("themes", "gnome");
	}

	return o;
}
