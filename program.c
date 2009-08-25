#include <stdlib.h>
#include <stdio.h>
#include "lib/card.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"
#include "keyboard_handler.h"
#include "mouse_handler.h"
#include "rendering.h"

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
	g_context = mem_context_create();
	g_solitaire = solitaire_theidiot(g_context, visual_settings_create(g_context));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Cardy 4");
	glutReshapeFunc(window_size_change);
	glutKeyboardFunc(window_key_press);
	glutSpecialFunc(window_special_key_press);
	glutMouseFunc(window_mouse);
	glutMotionFunc(window_mouse_move);
	glutDisplayFunc(render_scene);

	setup_render_context();
	setup_render_resources();

	glutMainLoop();

	mem_context_free(g_context);
	return 0;
}
