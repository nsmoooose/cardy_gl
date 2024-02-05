#include <unistd.h>
#include "api/mygl.h"
#include "api/render.h"
#include "game/backgrounds/rotating_triangles.h"

render_context *g_rcontext = 0;

void rendering_scene(void) {
	glMatrixMode(GL_MODELVIEW);
	render_scene_context(g_rcontext);
	check_gl_errors("render_scene");
	glutSwapBuffers();
}

void rendering_timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, rendering_timer, 0);
}

int main(int argc, char **argv) {
	g_rcontext = render_context_create();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("example-desktop");
	glutTimerFunc(1000 / 60, rendering_timer, 0);
	glutDisplayFunc(rendering_scene);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f, 1.0f, 5000.0f);

	g_rcontext->object = render_object_triangles();

	glutMainLoop();

	return 0;
}
