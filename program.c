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
mem_context *g_solcontext = 0;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	rsvg_init();

	g_context = mem_context_create();
	g_solcontext = mem_context_create();
	g_solitaire = solitaire_theidiot(g_solcontext, visual_settings_create(g_context));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Cardy 4");
	glutReshapeFunc(render_window_size_change);
	glutKeyboardFunc(window_key_press);
	glutSpecialFunc(window_special_key_press);
	glutMouseFunc(window_mouse);
	glutMotionFunc(window_mouse_move);
	glutDisplayFunc(render_scene);

	render_setup();

	glutMainLoop();

	mem_context_free(g_context);
	return 0;
}
