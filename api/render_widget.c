#include <stdlib.h>
#include "render_widget.h"

typedef struct {
	float top, left;
	float width, height;
} widget_style;

typedef struct {
	widget_style* style;
	render_selection_callback click;
} widget_data;

void widget_style_set_pos(render_object *object, float left, float top) {
	widget_data *d = object->data;
	d->style->left = left;
	d->style->top = top;
}

void widget_style_set_size(render_object *object, float width, float height) {
	widget_data *d = object->data;
	d->style->width = width;
	d->style->height = height;
}

void widget_style_set_image(render_object *object, RsvgHandle *h, char *svg_id) {
}

static void widget_free(render_event_args *event) {
	widget_data *d = event->object->data;
	free(d->style);
	free(d);
}

static render_object *widget_create(const char *id) {
	render_object *o = render_object_create(id);
	widget_data *d = calloc(1, sizeof(widget_data));
	d->style = calloc(1, sizeof(widget_style));
	o->data = d;
	o->free = widget_free;
	return o;
}

/* ----------------------------------------------------------------------------*/

static void widget_desktop_render(render_event_args *event, float delta) {
	GLint viewport[4];
	render_pick *pick;
	glGetIntegerv(GL_VIEWPORT, viewport);

	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(event->rcontext->pick) {
		pick = event->rcontext->pick;
		gluPickMatrix(pick->x, pick->y, pick->width, pick->height, pick->viewport);
	}
    glOrtho(0.0f, viewport[2], viewport[3], 0.0f, -200.0f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
}

render_object *widget_desktop(const char *id) {
	render_object *o = render_object_create(id);
	o->render = widget_desktop_render;
	return o;
}

/* ----------------------------------------------------------------------------*/

static void widget_window_render(render_event_args *event, float delta) {
	widget_data *d = event->object->data;

	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(d->style->left, d->style->top,
			d->style->left + d->style->width, d->style->top + d->style->height);

	glPushMatrix();
	glTranslatef(d->style->left, d->style->top, 0.0f);
}

static void widget_window_post_render(render_event_args *event, float delta) {
	glPopMatrix();
}

render_object *widget_window(const char *id) {
	render_object *o = widget_create(id);
	o->render = widget_window_render;
	o->post_render = widget_window_post_render;
	return o;
}

/* ----------------------------------------------------------------------------*/

static void widget_button_render(render_event_args *event, float delta) {
	widget_data *d = event->object->data;

	if(d->click) {
		glPushName(render_register_selection_callback(
					   event->rcontext, event->object, d->click, 0));
	}
	glColor3f(0.8f, 0.8f, 0.8f);
	glRectf(d->style->left, d->style->top,
			d->style->left + d->style->width,
			d->style->top + d->style->height);

	if(d->click) {
		glPopName();
	}
}

render_object *widget_button(const char *id) {
	render_object *o = widget_create(id);
	o->render = widget_button_render;
	return o;
}

void widget_button_set_text(render_object *object, char *text) {
}

void widget_button_set_callback(
	render_object *object, render_selection_callback callback) {
	widget_data *d = object->data;
	d->click = callback;
}
