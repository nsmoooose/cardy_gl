#include <unistd.h>
#include "api/render_widget.h"
#include "game/render_background.h"
#include "game/render_mainmenu.h"
#include "game/render_solitaire.h"
#include "game/render_topmenu.h"
#include "game/solitaire_theidiot.h"
#include "program.h"
#include "rendering.h"

void rendering_setup() {
	render_object *desktop;

	g_rcontext = render_context_create(g_context);
	g_rcontext->object = render_object_background(g_context);

	/*
	render_object_add_child(g_rcontext->object,	render_object_mainmenu());
	render_object_add_child(g_rcontext->object, render_object_topmenu());
	*/
	desktop = widget_desktop(0);
	render_object_add_child(g_rcontext->object, desktop);
}

void rendering_window_size_change(int width, int height) {
	GLfloat aspect;
	if(height == 0) {
		height = 1;
	}

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspect = (float)width/(float)height;
	gluPerspective(g_perspective_fov, aspect, g_perspective_near,
				   g_perspective_far);
}

void rendering_scene() {
	glMatrixMode(GL_MODELVIEW);
	glInitNames();
	render_scene_context(g_rcontext);
	check_gl_errors("render_scene");
	glutSwapBuffers();
}

void rendering_idle() {
	static int lastUpdate = 0;
	static int frames = 0;
	char buf[20];
	int currentTime;

	glutPostRedisplay();
	glutSwapBuffers();

	currentTime = glutGet(GLUT_ELAPSED_TIME);
	frames++;

	if ((currentTime - lastUpdate) >= 1000) {
		sprintf( buf, "Cardy 4.0 (fps: %d)", frames );
		glutSetWindowTitle(buf);
		frames = 0;
		lastUpdate = currentTime;
	}

	usleep(50000);
}
