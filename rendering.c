#include "lib/mygl.h"
#include "rendering.h"

float g_camera_zoom = -250.0f;
float g_camera_translateX = 0.0f;
float g_camera_translateY = 0.0f;

/* Build a vector of coordinates for a card. */
static GLfloat g_card_coords[8*3] = {
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

void setup_render_resources() {
	/* Generate normals. */

	/* Generate texture coordinates. */

	/* Build a display list for each card with
	 * a bitmap for each card.
	 */
}

void update_camera_pos() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(g_camera_translateX, g_camera_translateY, g_camera_zoom);
}

void render_solitaire(solitaire* sol) {
	int pile_index;
	int pile_count = sol->get_pile_count(sol);
	for(pile_index=0;pile_index<pile_count;++pile_index) {
		pile* pile = sol->get_pile(sol, pile_index);

		render_pile(pile);
	}
}

void render_pile(pile* pile) {
	/* Do a translation of our position for the pile. */
	glPushMatrix();
	glTranslatef(pile->origin[0], pile->origin[1], pile->origin[2]);

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
	int card_index;
	for(card_index=0;card_index<pile->card_count;++card_index) {
		render_card(pile, pile->first[card_index]);
	}
	glPopMatrix();

	glPopMatrix();
}

void render_card(pile* pile, card_proxy* proxy) {
	if(proxy->card == 0) {
		glColor3f(0.7f, 0.7f, 1.0f);
	}
	else {
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	glVertexPointer(3, GL_FLOAT, 0, g_card_coords);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, g_card_indexes);

	/* Do a translation of our position for the next card. */
	glTranslatef(0.0f, 0.0f, CARD_THICKNESS);
}

void render_desktop() {
	glClear(GL_COLOR_BUFFER_BIT);
}
