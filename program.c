#include <getopt.h>
#include <librsvg/rsvg.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api/card.h"
#include "api/mygl.h"
#include "game/render_solitaire.h"
#include "game/render_mainmenu.h"
#include "mouse_handler.h"
#include "rendering.h"

mem_context *g_context = 0;
render_context *g_rcontext = 0;

void window_key_press(unsigned char key, int x, int y) {
	render_process_keyboard_down(g_rcontext, key, glutGetModifiers(), x, y);
	glutPostRedisplay();
}

void window_special_key_press(int key, int x, int y) {
	render_process_keyboard_special_down(g_rcontext, key, glutGetModifiers(), x,
	                                     y);
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	int opt, width = 800, height = 600;
	char *separator;

	setlocale(LC_NUMERIC, "C");

	while ((opt = getopt(argc, argv, "dhg:")) != -1) {
		switch (opt) {
		case 'd':
			render_testing = true;
			break;
		case 'h':
			fprintf(stderr, "Usage: %s [-h] [-g] 300x200\n", argv[0]);
			return 0;
		case 'g':
			separator = strstr(optarg, "x");
			if (separator) {
				width = atoi(optarg);
				height = atoi(separator + 1);
			}
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [-h] [-g] 300x200\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	srand(time(NULL));

	g_context = mem_context_create();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Cardy 4");
	glutReshapeFunc(rendering_window_size_change);
	glutKeyboardFunc(window_key_press);
	glutSpecialFunc(window_special_key_press);
	glutMouseFunc(window_mouse);
	glutMotionFunc(window_mouse_move);
	glutDisplayFunc(rendering_scene);
	glutTimerFunc(1000 / 60, rendering_timer, 0);

	rendering_setup();

#ifdef WIN32
	/* If the desktop is using scaling you don't want all content
	   to be scaled bitmaps. It really looks uggly if you are scaling
	   things up 150%. */
	SetProcessDPIAware();
#endif

	glutMainLoop();

	mem_context_free(g_context);
	render_context_free(g_rcontext);
	return 0;
}
