#include <stdlib.h>
#include <stdio.h>
#include "lib/card.h"
#include "keyboard_handler.h"
#include "lib/gameengine.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"

static void render_pile(pile* pile) {
	glColor3f(0.0f, 0.7f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(pile->origin[0] - CARD_WIDTH/2.0f, pile->origin[1] + CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] + CARD_WIDTH/2.0f, pile->origin[1] + CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] + CARD_WIDTH/2.0f, pile->origin[1] - CARD_HEIGHT/2.0f);
	glVertex2f(pile->origin[0] - CARD_WIDTH/2.0f, pile->origin[1] - CARD_HEIGHT/2.0f);
	glEnd();
}

static void render_card(pile* pile, card_proxy* proxy) {
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

static void render_scene() {
	glClear(GL_COLOR_BUFFER_BIT);

	// print_solitaire_info(g_solitaire);

	int pile_index;
	int pile_count = g_solitaire->get_pile_count(g_solitaire);
	for(pile_index=0;pile_index<pile_count;++pile_index) {
		pile* pile = g_solitaire->get_pile(g_solitaire, pile_index);

		render_pile(pile);

		int card_index;
		for(card_index=0;card_index<pile->card_count;++card_index) {
			render_card(pile, pile->first[card_index]);
		}
	}

	glutSwapBuffers();
}

static void setup_render_context() {
	glClearColor(0.0f, 0.8f, 0.0f, 0.0f);
}

static void window_size_change(int width, int height) {
	GLfloat aspect;
	if(height == 0) {
		height = 1;
	}

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspect = (float)width/(float)height;
	gluPerspective(45.0, aspect, 1.0, 500.0);

	update_camera_pos();
}


int main(int argc, char* argv[]) {
	g_solitaire = solitaire_theidiot();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Cardy 4");
	glutReshapeFunc(window_size_change);
	glutKeyboardFunc(window_key_press);
	glutSpecialFunc(window_special_key_press);
	glutDisplayFunc(render_scene);

	setup_render_context();

	glutMainLoop();
	return 0;
}
