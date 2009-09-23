#include <unistd.h>
#include "../../lib/mygl.h"
#include "../../lib/render.h"
#include "../../lib/render_desktop.h"
#include "../../lib/render_widget.h"

render_context *g_rcontext = 0;

void rendering_scene() {
	glMatrixMode(GL_MODELVIEW);
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

int main(int argc, char** argv) {
	g_rcontext = render_context_create();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("example-desktop");
	glutIdleFunc(rendering_idle);
	glutDisplayFunc(rendering_scene);

	g_rcontext->object = render_object_desktop("background");
	render_object_add_child(g_rcontext->object, widget_desktop("id"));

	glutMainLoop();

	return 0;
}
