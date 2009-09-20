#include <math.h>
#include <stdlib.h>
#include "mygl.h"
#include "render.h"

/* Fun algorithms:

Growing grass.
* Up to 30 sections.
* width is the section no.
* length 25 per section.
* twist between each segment should be random between -2 -> 2. Same is used
for every one there after.
* different shades of green (some randomness)
* a new grass straw every 10 seconds.
* max 50 straws.
 */

typedef struct {
	float total_time;
	GLuint triangle;
} render_object_desktop_data;

static void render_triangle() {
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, -3.0f);
	glVertex2f(3.0, 3.0);
	glVertex2f(-3.0, 3.0);
	glEnd();
}

static void render_object_desktop_render(render_event_args *event, float delta) {
	render_object_desktop_data *i = event->object->data;
	int x, y;
	float distance;

	glClearColor(0.0f, 0.8f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -200.0f);

	for(x=-20;x<20;++x) {
		for(y=-20;y<20;++y) {
			distance = sqrt((float)(abs(x)*abs(x) + abs(y)*abs(y))) / 300.0f;

			glColor3f(0.0f, 0.8f + distance, 0.0f);
			glPushMatrix();
			glTranslatef(x * 10.0f, y * 10.0f, 0.0f);
			glRotatef(i->total_time * 10.0, 0.0f, 0.0f, 1.0f);
			glCallList(i->triangle);
			glPopMatrix();
		}
	}

	i->total_time += delta;
}

static void render_object_desktop_free(render_event_args *event) {
	render_object_desktop_data *i = event->object->data;
	glDeleteLists(i->triangle, 1);
	free(event->object->data);
}

render_object *render_object_desktop() {
	render_object *o = render_object_create("desktop");
	render_object_desktop_data *d = calloc(1, sizeof(render_object_desktop_data));
	o->data = d;
	o->render = render_object_desktop_render;
	o->free = render_object_desktop_free;

	d->triangle = glGenLists(1);
	glNewList(d->triangle, GL_COMPILE);
	render_triangle();
	glEndList();
	return o;
}
