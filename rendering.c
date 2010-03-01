#include <unistd.h>
#include "api/render_widget.h"
#include "game/backgrounds/rotating_triangles.h"
#include "game/render_mainmenu.h"
#include "game/render_topmenu.h"
#include "program.h"
#include "rendering.h"

void rendering_setup() {
	render_object *desktop;

	g_rcontext = render_context_create(g_context);
	g_rcontext->object = render_object_triangles(g_context);

	render_object_add_child(g_rcontext->object, render_object_create("placeholder"));

	desktop = widget_desktop("desktop");
	render_object_add_child(g_rcontext->object, desktop);
	render_object_topmenu(desktop);

	render_object_mainmenu(desktop);
}

void rendering_window_size_change(int width, int height) {
	glViewport(0, 0, width, height);
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
