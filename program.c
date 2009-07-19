#include <stdlib.h>
#include <stdio.h>
#include "lib/card.h"
#include "keyboard_handler.h"
#include "rendering.h"
#include "lib/gameengine.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"

static void render_scene() {
	render_desktop();
	render_solitaire(g_solitaire);
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
