#include "lib/mygl.h"
#include "rendering.h"

float g_camera_zoom = -250.0f;
float g_camera_translateX = 0.0f;
float g_camera_translateY = 0.0f;

void update_camera_pos() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(g_camera_translateX, g_camera_translateY, g_camera_zoom);
}

void render_pile(pile* pile) {
	glColor3f(0.0f, 0.7f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(pile->origin[0] - CARD_WIDTH/2.0f, pile->origin[1] + CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] + CARD_WIDTH/2.0f, pile->origin[1] + CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] + CARD_WIDTH/2.0f, pile->origin[1] - CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] - CARD_WIDTH/2.0f, pile->origin[1] - CARD_HEIGHT/2.0f);
	glEnd();
}

void render_card(pile* pile, card_proxy* proxy) {
	if(proxy->card == 0) {
		glColor3f(0.7f, 0.7f, 1.0f);
	}
	else {
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glBegin(GL_QUADS);
	glVertex2f(pile->origin[0] - CARD_WIDTH/2.0f, pile->origin[1] + CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] + CARD_WIDTH/2.0f, pile->origin[1] + CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] + CARD_WIDTH/2.0f, pile->origin[1] - CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] - CARD_WIDTH/2.0f, pile->origin[1] - CARD_HEIGHT/2.0f);
	glEnd();
}

void render_solitaire(solitaire* sol) {
	int pile_index;
	int pile_count = sol->get_pile_count(sol);
	for(pile_index=0;pile_index<pile_count;++pile_index) {
		pile* pile = sol->get_pile(sol, pile_index);

		render_pile(pile);

		int card_index;
		for(card_index=0;card_index<pile->card_count;++card_index) {
			render_card(pile, pile->first[card_index]);
		}
	}
}

void render_desktop() {
	glClear(GL_COLOR_BUFFER_BIT);
}
