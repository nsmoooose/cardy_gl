#include <stdio.h>
#include "lib/mygl.h"
#include "mouse_handler.h"
#include "rendering.h"

#define MAX_SELECTION 2000

static void process_selection(GLint hits, GLuint* selections) {
	int count = selections[0];

	printf("Number of hits: %d\n", hits);
	int i=0;
	for(;i<20;++i) {
		printf("0x%x\n", selections[i]);
	}

	pile* selected_pile = 0;
	card_proxy* selected_proxy = 0;

	int index=0;
	int hit=0;
	for(;hit<hits;++hit) {
		if(selections[index] == 1) {
			selected_pile = (pile*)selections[index+3];
		}
		else if(selections[index] == 2) {
			selected_pile = (pile*)selections[index+3];
			card_proxy* selected_proxy = (card_proxy*)selections[index+4];
		}
		index += selections[index] + 3;
	}

	if(selected_pile && selected_proxy && selected_pile->card_action) {
		selected_pile->card_action(g_solitaire, selected_pile, selected_proxy);
	}
	else if(selected_pile && selected_pile->pile_action) {
		selected_pile->pile_action(g_solitaire, selected_pile);
	}
}

void window_mouse(int button, int state, int x, int y) {
	printf("Button: %d, state: %d, position (%d,%d)\n", button, state, x, y);

	/* Do nothing unless it is a left button and a mouse
	 * down event.
	 */
	if(button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
		return;
	}

	GLuint selections[MAX_SELECTION];
	glSelectBuffer(MAX_SELECTION, selections);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y + viewport[1], 1, 1, viewport);

	GLfloat aspect = (float)viewport[2] / (float)viewport[3];
	gluPerspective(g_perspective_fov, aspect, g_perspective_near, g_perspective_far);

	render_scene();

	GLint hits = glRenderMode(GL_RENDER);
	if(hits == -1) {
		printf("-1 hits. Selection buffer not large enough.");
	}
	else if(hits > 0) {
		process_selection(hits, selections);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

