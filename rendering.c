#include <unistd.h>
#include "lib/render_desktop.h"
#include "lib/render_mainmenu.h"
#include "lib/render_solitaire.h"
#include "lib/solitaire_theidiot.h"
#include "program.h"
#include "rendering.h"

void rendering_setup() {
	glEnableClientState(GL_VERTEX_ARRAY);
	g_rcontext = render_context_create(g_context);
	g_rcontext->object = render_object_desktop(g_context);
	render_object_add_child(g_rcontext->object,	render_object_mainmenu());
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

	render_update_camera_pos();
}

void rendering_scene() {
	static int lastUpdate = 0;
	float delta;
	int currentTime = glutGet(GLUT_ELAPSED_TIME);;

	delta = ((float)(currentTime - lastUpdate)) / 1000.0f;
	glMatrixMode(GL_MODELVIEW);
	glInitNames();
	render_scene_context(g_rcontext, delta);
	check_gl_errors("render_scene");
	glutSwapBuffers();

	lastUpdate = currentTime;
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
		sprintf( buf, "FPS: %d", frames );
		glutSetWindowTitle( buf );
		frames = 0;
		lastUpdate = currentTime;
	}

	usleep(50000);
}
