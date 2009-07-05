#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>

void render_scene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}

void setup_render_context() {
	glClearColor(0.0f, 0.8f, 0.0f, 0.0f);
}

void window_size_change(int width, int height) {
}

void window_key_press(unsigned char key, int x, int y) {
	printf("Keyboard pressed: %c\n", key);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Cardy 4");
	glutReshapeFunc(window_size_change);
	glutKeyboardFunc(window_key_press);
	glutDisplayFunc(render_scene);

	setup_render_context();

	glutMainLoop();
	return 0;
}
