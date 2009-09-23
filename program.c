#include <getopt.h>
#include <librsvg/rsvg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/card.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"
#include "lib/render_solitaire.h"
#include "mouse_handler.h"
#include "rendering.h"

mem_context *g_context = 0;
render_context *g_rcontext = 0;

void window_key_press(unsigned char key, int x, int y) {
	render_process_keyboard_down(g_rcontext, key, glutGetModifiers(), x, y);
	glutPostRedisplay();
}

void window_special_key_press(int key, int x, int y) {
	render_process_keyboard_special_down(g_rcontext, key, glutGetModifiers(), x, y);
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	int opt, width = 800, height = 600;
	char *separator;

	while ((opt = getopt(argc, argv, "hg:")) != -1) {
		switch (opt) {
		case 'h':
			fprintf(stderr, "Usage: %s [-h] [-g] 300x200\n",
					argv[0]);
			return 0;
		case 'g':
			separator = strstr(optarg, "x");
			if(separator) {
				width = atoi(optarg);
				height = atoi(separator + 1);
			}
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [-h] [-g] 300x200\n",
					argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	srand(time(NULL));
	rsvg_init();

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
	glutIdleFunc(rendering_idle);
	glutDisplayFunc(rendering_scene);

	rendering_setup();

	glutMainLoop();

	mem_context_free(g_context);
	render_context_free(g_rcontext);
	return 0;
}
