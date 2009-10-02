#include <stdlib.h>
#include "expression.h"
#include "render_widget.h"

#define style_key_width "width"
#define style_key_height "height"
#define style_key_top "top"
#define style_key_left "left"
#define style_key_backcolor_red "backcolor_red"
#define style_key_backcolor_green "backcolor_green"
#define style_key_backcolor_blue "backcolor_blue"
#define style_key_backcolor_alpha "backcolor_alpha"

typedef struct {
	float red, green, blue, alpha;
} widget_color;

struct widget_style_St {
	mem_context *mc;
	GHashTable *expressions;

	GLuint back_texture;
	render_selection_callback click;
};

typedef struct {
	widget_style* style;
} widget_data;

widget_style *widget_get_default_style(render_object *object) {
	widget_data *d = object->data;
	return d->style;
}

static float widget_get_style_value(widget_style *style, const char *key, float def) {
	expression *exp = g_hash_table_lookup(style->expressions, key);
	if(exp) {
		return expression_execute(0, exp);
	}
	return def;
}

void widget_style_set_backcolor(
	widget_style *style, float red, float green, float blue, float alpha) {
	expression *red_exp = expression_const(style->mc, red);
	expression *green_exp = expression_const(style->mc, green);
	expression *blue_exp = expression_const(style->mc, blue);
	expression *alpha_exp = expression_const(style->mc, alpha);
	g_hash_table_insert(style->expressions, style_key_backcolor_red, red_exp);
	g_hash_table_insert(style->expressions, style_key_backcolor_blue, green_exp);
	g_hash_table_insert(style->expressions, style_key_backcolor_green, blue_exp);
	g_hash_table_insert(style->expressions, style_key_backcolor_alpha, alpha_exp);
}

void widget_style_set_pos(widget_style *style, float left, float top) {
	expression *left_exp = expression_const(style->mc, left);
	expression *top_exp = expression_const(style->mc, top);
	g_hash_table_insert(style->expressions, style_key_left, left_exp);
	g_hash_table_insert(style->expressions, style_key_top, top_exp);
}

void widget_style_set_size(widget_style *style, float width, float height) {
	expression *width_exp = expression_const(style->mc, width);
	expression *height_exp = expression_const(style->mc, height);
	g_hash_table_insert(style->expressions, style_key_width, width_exp);
	g_hash_table_insert(style->expressions, style_key_height, height_exp);
}

void widget_style_set_image(
	widget_style *style, RsvgHandle *h, char *svg_id, int width, int height) {
	glGenTextures(1, &style->back_texture);
	render_svg_texture(h, style->back_texture, svg_id, width, height);
}

void widget_style_set_click_callback(
	widget_style *style, render_selection_callback callback) {
	style->click = callback;
}

static void widget_free(render_event_args *event) {
	widget_data *d = event->object->data;
	g_hash_table_unref(d->style->expressions);
	mem_context_free(d->style->mc);
	free(d->style);
	free(d);
}

static render_object *widget_create(const char *id) {
	render_object *o = render_object_create(id);
	widget_data *d = calloc(1, sizeof(widget_data));
	d->style = calloc(1, sizeof(widget_style));
	o->data = d;
	o->free = widget_free;

	d->style->expressions = g_hash_table_new(g_str_hash, g_str_equal);
	d->style->mc = mem_context_create();
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
	widget_color bc;

	float left = widget_get_style_value(d->style, style_key_left, 0.0f);
	float top = widget_get_style_value(d->style, style_key_top, 0.0f);
	float width = widget_get_style_value(d->style, style_key_width, 0.0f);
	float height = widget_get_style_value(d->style, style_key_height, 0.0f);
	bc.red = widget_get_style_value(d->style, style_key_backcolor_red, 1.0f);
	bc.green = widget_get_style_value(d->style, style_key_backcolor_green, 1.0f);
	bc.blue = widget_get_style_value(d->style, style_key_backcolor_blue, 1.0f);
	bc.alpha = widget_get_style_value(d->style, style_key_backcolor_alpha, 1.0f);

	glColor4f(bc.red, bc.green, bc.blue, bc.alpha);
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
	glVertex2f(left, top);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(left + width, top);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(left + width, top + height);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(left, top + height);
	glEnd();
	if(d->style->back_texture) {
		glDisable(GL_TEXTURE_2D);
	}
	if(d->style->click) {
		glPopName();
	}
	glPushMatrix();
	glTranslatef(left, top, 0.0f);
}

static void widget_generic_post_render(render_event_args *event, float delta) {
	glPopMatrix();
}

render_object *widget_generic(const char *id) {
	render_object *o = widget_create(id);
	o->render = widget_generic_render;
	o->post_render = widget_generic_post_render;
	return o;
}

/* ----------------------------------------------------------------------------*/

void widget_button_set_text(render_object *object, char *text) {
}
