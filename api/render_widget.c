#include <stdlib.h>
#include "render_widget.h"

typedef struct {
	float red, green, blue, alpha;
} widget_color;

typedef struct {
	float top, left;
	float width, height;

	widget_color back_color;
	GLuint back_texture;
	render_selection_callback click;
} widget_style;

typedef struct {
	widget_style* style;
} widget_data;

void widget_style_set_backcolor(
	render_object *object, float red, float green, float blue, float alpha) {
	widget_data *d = object->data;

	d->style->back_color.red = red;
	d->style->back_color.green = green;
	d->style->back_color.blue = blue;
	d->style->back_color.alpha = alpha;
}

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

void widget_style_set_image(
	render_object *object, RsvgHandle *h, char *svg_id, int width, int height) {
	widget_data *d = object->data;
	glGenTextures(1, &d->style->back_texture);
	render_svg_texture(h, d->style->back_texture, svg_id, width, height);
}

void widget_style_set_click_callback(
	render_object *object, render_selection_callback callback) {
	widget_data *d = object->data;
	d->style->click = callback;
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

static void widget_generic_render(render_event_args *event, float delta) {
	widget_data *d = event->object->data;
	widget_color *bc = &d->style->back_color;

	glColor4f(bc->red, bc->green, bc->blue, bc->alpha);
	if(d->style->click) {
		glPushName(render_register_selection_callback(
					   event->rcontext, event->object, d->style->click, 0));
	}
	if(d->style->back_texture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, d->style->back_texture);
	}
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(d->style->left, d->style->top);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(d->style->left + d->style->width, d->style->top);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(d->style->left + d->style->width, d->style->top + d->style->height);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(d->style->left, d->style->top + d->style->height);
	glEnd();
	if(d->style->back_texture) {
		glDisable(GL_TEXTURE_2D);
	}
	if(d->style->click) {
		glPopName();
	}
	glPushMatrix();
	glTranslatef(d->style->left, d->style->top, 0.0f);
}

static void widget_generic_post_render(render_event_args *event, float delta) {
	glPopMatrix();
}

render_object *widget_generic(const char *id) {
	render_object *o = widget_create(id);
	widget_data *d = o->data;
	d->style->back_color.red = 1.0f;
	d->style->back_color.green = 1.0f;
	d->style->back_color.blue = 1.0f;
	d->style->back_color.alpha = 1.0f;
	o->render = widget_generic_render;
	o->post_render = widget_generic_post_render;
	return o;
}

/* ----------------------------------------------------------------------------*/

void widget_button_set_text(render_object *object, char *text) {
}
