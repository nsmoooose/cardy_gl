#include <unistd.h>
#include "api/render_widget.h"
#include "client/ui.h"
#include "client/program.h"
#include "client/rendering.h"

void rendering_setup(void) {
	g_rcontext = render_context_create();
	g_rcontext->object = render_object_create("root");

	ui_scene_main(g_rcontext);
}

void rendering_window_size_change(int width, int height) {
	glViewport(0, 0, width, height);
}

void rendering_scene(void) {
	glMatrixMode(GL_MODELVIEW);
	glInitNames();
	render_scene_context(g_rcontext);
	check_gl_errors("render_scene");
	glutSwapBuffers();
}

void rendering_timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, rendering_timer, 0);
}
