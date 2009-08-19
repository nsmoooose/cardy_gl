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
	if(proxy->card == 0) {
		glColor3f(0.7f, 0.7f, 1.0f);
	}
	else {
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	if(g_selected_card == proxy) {
		glColor3f(1.0f, 0.7f, 0.7f);
	}

	glPushName((GLuint)proxy);
	glVertexPointer(3, GL_FLOAT, 0, g_card_coords);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, g_card_indexes);
	glPopName();

	/* Do a translation of our position for the next card. */
	glTranslatef(pile->translateX, pile->translateY, CARD_THICKNESS);
}

void render_desktop() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void render_scene() {
	glInitNames();
	glPushName(0);

	render_desktop();
	render_solitaire(g_solitaire);
	glutSwapBuffers();
}
