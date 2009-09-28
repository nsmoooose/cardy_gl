#include <unistd.h>
#include "../../api/mygl.h"
#include "../../api/render.h"
#include "../../api/render_widget.h"
#include "../../game/render_background.h"

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

void button1_callback(render_event_args *event, void *data) {
	printf("button1\n");
}

void button2_callback(render_event_args *event, void *data) {
	printf("button2\n");
}

int main(int argc, char** argv) {
	render_object *desktop, *window, *button1, *button2;
	g_rcontext = render_context_create();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("example-desktop");
	glutIdleFunc(rendering_idle);
	glutDisplayFunc(rendering_scene);

	g_rcontext->object = render_object_background();

	desktop = widget_desktop("id");
	render_object_add_child(g_rcontext->object, desktop);

	window = widget_window(0);
	widget_style_set_pos(window, 300, 100);
	widget_style_set_size(window, 100, 100);
	render_object_add_child(desktop, window);

	button1 = widget_button(0);
	widget_style_set_pos(button1, 5, 5);
	widget_style_set_size(button1, 90, 40);
	widget_button_set_callback(button1, button1_callback);
	render_object_add_child(window, button1);

	button2 = widget_button(0);
	widget_style_set_pos(button2, 5, 55);
	widget_style_set_size(button2, 90, 40);
	widget_button_set_callback(button2, button2_callback);
	render_object_add_child(window, button2);

	glutMainLoop();

	return 0;
}
