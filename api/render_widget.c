#include <cairo.h>
#include <stdlib.h>
#include <string.h>
#include "api/expression.h"
#include "api/render_widget.h"

typedef struct {
	float red, green, blue, alpha;
} widget_color;

struct widget_style_St {
	expression_context *ec;

	GLuint texture;
	render_selection_callback click;

	int image_width, image_height;
	char *font_face;
	float font_size;
	float text_color[4];
};

typedef struct {
	widget_style *style;
	void *data;
} widget_data;

void widget_set_data(render_object *widget, void *data) {
	widget_data *d = widget->data;
	d->data = data;
}

void *widget_get_data(render_object *widget) {
	widget_data *d = widget->data;
	return d->data;
}

void widget_add_state(render_object *widget, const char *state) {}

void widget_remove_state(render_object *widget, const char *state) {}

bool widget_state_active(render_object *widget, const char *state) {
	return false;
}

widget_style_transition *widget_transition_create(widget_transition_state state,
                                                  float length,
                                                  expression *exp) {
	return 0;
}

void widget_style_add_transition(widget_style *style, const char *state,
                                 const char *property,
                                 widget_style_transition *t) {}

widget_style *widget_get_default_style(render_object *object) {
	widget_data *d = object->data;
	return d->style;
}

expression_context *widget_style_get_expression_context(widget_style *style) {
	return style->ec;
}

static float widget_get_style_value(widget_style *style, const char *key,
                                    float def) {
	expression *exp = expression_context_get(style->ec, key);
	if (exp) {
		return expression_execute(style->ec, exp);
	}
	return def;
}

void widget_style_set_backcolor(widget_style *style, float red, float green,
                                float blue, float alpha) {
	expression *red_exp = expression_const(red);
	expression *green_exp = expression_const(green);
	expression *blue_exp = expression_const(blue);
	expression *alpha_exp = expression_const(alpha);
	expression_context_set(style->ec, style_key_backcolor_red, red_exp);
	expression_context_set(style->ec, style_key_backcolor_blue, green_exp);
	expression_context_set(style->ec, style_key_backcolor_green, blue_exp);
	expression_context_set(style->ec, style_key_backcolor_alpha, alpha_exp);
}

void widget_style_set_pos(widget_style *style, float left, float top) {
	expression *left_exp = expression_const(left);
	expression *top_exp = expression_const(top);
	expression_context_set(style->ec, style_key_left, left_exp);
	expression_context_set(style->ec, style_key_top, top_exp);
}

void widget_style_set_left(widget_style *style, const char *expr) {
	expression_context_set(style->ec, style_key_left,
	                       expression_parse(0, expr));
}

void widget_style_set_top(widget_style *style, const char *expr) {
	expression_context_set(style->ec, style_key_top, expression_parse(0, expr));
}

expression *widget_style_get_top_expression(widget_style *style) {
	return expression_context_get(style->ec, style_key_top);
}

expression *widget_style_get_left_expression(widget_style *style) {
	return expression_context_get(style->ec, style_key_left);
}

void widget_style_set_width(widget_style *style, const char *expr) {
	expression_context_set(style->ec, style_key_width,
	                       expression_parse(0, expr));
}

void widget_style_set_height(widget_style *style, const char *expr) {
	expression_context_set(style->ec, style_key_height,
	                       expression_parse(0, expr));
}

void widget_style_set_size(widget_style *style, float width, float height) {
	expression *width_exp = expression_const(width);
	expression *height_exp = expression_const(height);
	expression_context_set(style->ec, style_key_width, width_exp);
	expression_context_set(style->ec, style_key_height, height_exp);
}

expression *widget_style_get_width_expression(widget_style *style) {
	return expression_context_get(style->ec, style_key_width);
}

expression *widget_style_get_height_expression(widget_style *style) {
	return expression_context_get(style->ec, style_key_height);
}

void widget_style_set_rotation(widget_style *style, const char *expr) {
	expression_context_set(style->ec, style_key_rotation,
	                       expression_parse(0, expr));
}

expression *widget_style_get_rotation_expression(widget_style *style) {
	return expression_context_get(style->ec, style_key_rotation);
}

void widget_style_set_image(widget_style *style, RsvgHandle *h, char *svg_id,
                            int width, int height) {
	glGenTextures(1, &style->texture);
	render_svg_texture(h, style->texture, svg_id, width, height);
}

void widget_style_set_click_callback(widget_style *style,
                                     render_selection_callback callback) {
	style->click = callback;
}

static void widget_free(render_event_args *event) {
	widget_data *d = event->object->data;
	expression_context_free(d->style->ec);
	if (d->style->texture) {
		glDeleteTextures(1, &d->style->texture);
	}
	free(d->style->font_face);
	free(d->style);
	free(d);
}

static render_object *widget_create(const char *id) {
	render_object *o = render_object_create(id);
	widget_data *d = calloc(1, sizeof(widget_data));
	d->style = calloc(1, sizeof(widget_style));
	d->style->ec = expression_context_create();
	o->data = d;
	o->free = widget_free;

	expression_context_set(d->style->ec, style_key_render_time,
	                       expression_pointer(&o->render_time));
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
	if (event->rcontext->pick) {
		pick = event->rcontext->pick;
		gluPickMatrix(pick->x, pick->y, pick->width, pick->height,
		              pick->viewport);
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
	GLint viewport[4];
	float left, top, width, height;
	float rotation;
	widget_data *d = event->object->data;
	widget_color bc;
	widget_style *style = widget_get_default_style(event->object);

	glGetIntegerv(GL_VIEWPORT, viewport);
	expression_context_set(style->ec, style_key_viewport_width,
	                       expression_const(viewport[2]));
	expression_context_set(style->ec, style_key_viewport_height,
	                       expression_const(viewport[3]));

	left = widget_get_style_value(d->style, style_key_left, 0.0f);
	top = widget_get_style_value(d->style, style_key_top, 0.0f);
	width = widget_get_style_value(d->style, style_key_width, 0.0f);
	height = widget_get_style_value(d->style, style_key_height, 0.0f);
	rotation = widget_get_style_value(d->style, style_key_rotation, 0.0f);
	bc.red = widget_get_style_value(d->style, style_key_backcolor_red, 1.0f);
	bc.green =
		widget_get_style_value(d->style, style_key_backcolor_green, 1.0f);
	bc.blue = widget_get_style_value(d->style, style_key_backcolor_blue, 1.0f);
	bc.alpha =
		widget_get_style_value(d->style, style_key_backcolor_alpha, 1.0f);

	/* Push the matrix for this control and child controls. */
	glPushMatrix();

	glColor4f(bc.red, bc.green, bc.blue, bc.alpha);
	if (d->style->click) {
		glPushName(render_register_selection_callback(
			event->rcontext, event->object, d->style->click, 0));
	}
	if (d->style->texture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, d->style->texture);
	}

	if (rotation != 0.0f) {
		glTranslatef(left + width / 2.0f, top + height / 2.0f, 0.0f);
		glRotatef(rotation, 0.0f, 0.0f, 1.0f);
		glTranslatef(-(left + width / 2.0f), -(top + height / 2.0f), 0.0f);
	}
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(left, top);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(left + width, top);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(left + width, top + height);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(left, top + height);
	glEnd();
	if (d->style->texture) {
		glDisable(GL_TEXTURE_2D);
	}
	if (d->style->click) {
		glPopName();
	}
	glTranslatef(left, top, 0.0f);
}

static void widget_generic_post_render(render_event_args *event, float delta) {
	/* Restore state so parent controls get the proper matrix. */
	glPopMatrix();
}

render_object *widget_generic(const char *id) {
	render_object *o = widget_create(id);
	o->render = widget_generic_render;
	o->post_render = widget_generic_post_render;
	return o;
}

/* ----------------------------------------------------------------------------*/

void widget_style_set_image_size(widget_style *style, int width, int height) {
	style->image_width = width;
	style->image_height = height;
}

void widget_style_set_font_face(widget_style *style, const char *name) {
	style->font_face = strdup(name);
}

void widget_style_set_font_size(widget_style *style, float size) {
	style->font_size = size;
}

void widget_style_set_text_color(widget_style *style, float red, float green,
                                 float blue, float alpha) {
	style->text_color[0] = red;
	style->text_color[1] = green;
	style->text_color[2] = blue;
	style->text_color[3] = alpha;
}

void widget_style_set_text(widget_style *style, const char *text) {
	cairo_t *cr;
	cairo_surface_t *cairo_surface;
	unsigned char *cairo_data;
	int stride;

	stride =
		cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, style->image_width);
	cairo_data = (unsigned char *)calloc(stride * style->image_height, 1);
	cairo_surface = cairo_image_surface_create_for_data(
		cairo_data, CAIRO_FORMAT_ARGB32, style->image_width,
		style->image_height, stride);

	cr = cairo_create(cairo_surface);

	/* Set the background color of the bitmap generated. */
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
	cairo_rectangle(cr, 0, 0, style->image_width, style->image_height);
	cairo_fill(cr);

	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_set_source_rgba(cr, style->text_color[0], style->text_color[1],
	                      style->text_color[2], style->text_color[3]);
	if (style->font_face) {
		cairo_select_font_face(cr, style->font_face, CAIRO_FONT_SLANT_NORMAL,
		                       CAIRO_FONT_WEIGHT_NORMAL);
	}

	cairo_set_font_size(cr, style->font_size);
	cairo_move_to(cr, 0, style->image_height / 2);
	cairo_show_text(cr, text);

	glGenTextures(1, &style->texture);
	glBindTexture(GL_TEXTURE_2D, style->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, style->image_width,
	             style->image_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, cairo_data);

	free(cairo_data);
	cairo_destroy(cr);
	cairo_surface_destroy(cairo_surface);
}
