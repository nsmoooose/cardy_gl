#include "lib/render_desktop.h"
#include "lib/render_mainmenu.h"
#include "lib/render_solitaire.h"
#include "lib/solitaire_theidiot.h"
#include "program.h"
#include "rendering.h"

void render_setup() {
	glEnableClientState(GL_VERTEX_ARRAY);
	g_rcontext = render_context_create(g_context);
	g_rcontext->object = render_object_desktop(g_context);
	render_object_add_child(g_rcontext->object,
							render_object_solitaire(g_context));
	render_object_add_child(g_rcontext->object,
							render_object_mainmenu(&g_solcontext));

	g_solitaire = solitaire_theidiot(g_solcontext, g_visual_settings);
}

void render_window_size_change(int width, int height) {
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

void render_scene() {
	glMatrixMode(GL_MODELVIEW);

	glInitNames();

	render_scene_context(g_rcontext, 0.0f);

	check_gl_errors("render_scene");

	glutSwapBuffers();
}
