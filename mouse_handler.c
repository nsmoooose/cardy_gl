#include <stdio.h>
#include "lib/mygl.h"
#include "lib/render_solitaire.h"
#include "mouse_handler.h"
#include "program.h"
#include "rendering.h"

#define MAX_SELECTION 2000

void window_mouse(int button, int state, int x, int y) {
	GLuint selections[MAX_SELECTION];
	GLint viewport[4], hits;
	GLfloat aspect;

	/* printf("Button: %d, state: %d, position (%d,%d)\n", button, state, x, y); */

	/* Do nothing unless it is a left button and a mouse
	 * down event.
	 */
	if(button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
		return;
	}

	glSelectBuffer(MAX_SELECTION, selections);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y + viewport[1], 1, 1, viewport);

	aspect = (float)viewport[2] / (float)viewport[3];
	gluPerspective(g_perspective_fov, aspect, g_perspective_near,
				   g_perspective_far);

	check_gl_errors("window_mouse (selection setup)");

	rendering_scene();

	hits = glRenderMode(GL_RENDER);
	if(hits == -1) {
		fprintf(stderr, "-1 hits. Selection buffer not large enough.\n");
	}
	else if(hits > 0) {
		render_process_selections(g_rcontext, hits, selections);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void window_mouse_move(int x, int y) {
	/*
	printf("Dragging.\n");
	*/
}
