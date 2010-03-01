#include <math.h>
#include <stdlib.h>
#include "../../api/ease.h"
#include "../../api/mygl.h"
#include "../../api/render.h"

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
} render_object_triangles_data;

static void render_triangle() {
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, -3.0f);
	glVertex2f(3.0, 3.0);
	glVertex2f(-3.0, 3.0);
	glEnd();
}

static void render_object_triangles_render(render_event_args *event, float delta) {
	render_object_triangles_data *i = event->object->data;
	int x, y;
	float distance, green;
	GLint viewport[4];
	GLfloat aspect;

	green = ease_time_protect4f(
		ease_quad_out, i->total_time, 0.0f, 0.6f, 20.0f);

	glGetIntegerv(GL_VIEWPORT, viewport);
	if(viewport[3] == 0) {
		viewport[3] = 1;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspect = (float)viewport[2]/(float)viewport[3];
	gluPerspective(45.0f, aspect, 1.0f, 400.0f);

	glClearColor(0.0f, green, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -200.0f);

	for(x=-20;x<20;++x) {
		for(y=-20;y<20;++y) {
			distance = sqrt((float)(abs(x)*abs(x) + abs(y)*abs(y))) / 300.0f;

			glColor3f(0.0f, green + distance, 0.0f);
			glPushMatrix();
			glTranslatef(x * 10.0f, y * 10.0f, 0.0f);
			glRotatef(i->total_time * 10.0, 0.0f, 0.0f, 1.0f);
			glCallList(i->triangle);
			glPopMatrix();
		}
	}

	i->total_time += delta;
}

static void render_object_triangles_free(render_event_args *event) {
	render_object_triangles_data *i = event->object->data;
	glDeleteLists(i->triangle, 1);
	free(event->object->data);
}

render_object *render_object_triangles() {
	render_object *o = render_object_create("background");
	render_object_triangles_data *d = calloc(1, sizeof(render_object_triangles_data));
	o->data = d;
	o->render = render_object_triangles_render;
	o->free = render_object_triangles_free;

	d->triangle = glGenLists(1);
	glNewList(d->triangle, GL_COMPILE);
	render_triangle();
	glEndList();
	return o;
}
