#include <math.h>
#include <stdlib.h>
#include "client/ui.h"
#include "client/render_solved.h"

const char *render_object_solved_id = "solitaire_solved";

typedef struct {
	float total_time;
} internal;

static bool render_object_solved_keyboard_down(render_event_args *event,
                                               unsigned char key, int modifiers,
                                               int x, int y) {
	switch (key) {
	case 27:
		ui_scene_main(event->rcontext);
		return true;
	}
	return false;
}

static void render_object_solved_render(render_event_args *event, float delta) {
	internal *i = event->object->data;

	glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -200.0f + (sin(i->total_time) * 50));

	glColor3f(1.0f, 0.8f, 0.0f);
	glRotatef(i->total_time * 30.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -10.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, -30.0f);
	glVertex2f(30.0, 30.0);
	glVertex2f(-30.0, 30.0);
	glEnd();

	glColor3f(1.0f, 0.8f, 0.0f);
	glTranslatef(0.0f, 20.0f, 0.0f);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, -30.0f);
	glVertex2f(30.0, 30.0);
	glVertex2f(-30.0, 30.0);
	glEnd();

	glPopMatrix();

	i->total_time += delta;
}

static void render_object_solved_free(render_event_args *event) {
	free(event->object->data);
}

render_object *render_object_solved(void) {
	render_object *o = render_object_create(render_object_solved_id);
	o->data = calloc(1, sizeof(internal));
	o->keyboard_down = render_object_solved_keyboard_down;
	o->render = render_object_solved_render;
	o->free = render_object_solved_free;
	return o;
}
