#include <stdlib.h>
#include <stdio.h>
#include "lib/card.h"
#include "keyboard_handler.h"
#include "mouse_handler.h"
#include "rendering.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"

static void setup_render_context() {
	glClearColor(0.0f, 0.8f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
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
	gluPerspective(g_perspective_fov, aspect, g_perspective_near, g_perspective_far);

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
	glutMouseFunc(window_mouse);
	glutDisplayFunc(render_scene);

	setup_render_context();

	glutMainLoop();
	return 0;
}
