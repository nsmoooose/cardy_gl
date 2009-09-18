#include <librsvg/rsvg.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib/card.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"
#include "lib/render_solitaire.h"
#include "keyboard_handler.h"
#include "mouse_handler.h"
#include "rendering.h"

mem_context *g_context = 0;
render_context *g_rcontext = 0;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	rsvg_init();

	g_context = mem_context_create();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
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
