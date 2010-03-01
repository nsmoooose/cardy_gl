#include <unistd.h>
#include "../../api/mygl.h"
#include "../../api/render.h"
#include "../../api/render_widget.h"
#include "../../game/backgrounds/rotating_triangles.h"

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
	widget_style *style;
	g_rcontext = render_context_create();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("example-desktop");
	glutIdleFunc(rendering_idle);
	glutDisplayFunc(rendering_scene);

	g_rcontext->object = render_object_triangles();

	desktop = widget_desktop("id");
	render_object_add_child(g_rcontext->object, desktop);

	window = widget_generic(0);
	style = widget_get_default_style(window);
	widget_style_set_pos(style, 300, 100);
	widget_style_set_size(style, 100, 100);
	render_object_add_child(desktop, window);

	button1 = widget_generic(0);
	style = widget_get_default_style(button1);
	widget_style_set_pos(style, 5, 5);
	widget_style_set_size(style, 90, 40);
	widget_style_set_click_callback(style, button1_callback);
	render_object_add_child(window, button1);

	button2 = widget_generic(0);
	style = widget_get_default_style(button2);
	widget_style_set_pos(style, 5, 55);
	widget_style_set_size(style, 90, 40);
	widget_style_set_click_callback(style, button2_callback);
	render_object_add_child(window, button2);

	glutMainLoop();

	return 0;
}
