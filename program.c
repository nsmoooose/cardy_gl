#include <stdlib.h>
#include <stdio.h>
#include "card.h"
#include "keyboard_handler.h"
#include "mygl.h"
#include "solitaire_theidiot.h"

/** Global instance of the one and only solitaire that is currently
 *  running.
 */
solitaire* g_solitaire = 0;

static void render_scene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}

static void setup_render_context() {
	glClearColor(0.0f, 0.8f, 0.0f, 0.0f);
}

static void window_size_change(int width, int height) {
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
