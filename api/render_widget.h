#ifndef __RENDER_WIDGET_H__
#define __RENDER_WIDGET_H__

#include "expression.h"
#include "render.h"

struct widget_style_St;
typedef struct widget_style_St widget_style;

widget_style *widget_get_default_style(render_object *object);

expression_context *widget_style_get_expression_context(widget_style *style);

void widget_style_set_backcolor(
	widget_style *style, float red, float green, float blue, float alpha);

void widget_style_set_pos(widget_style *style, float left, float top);
expression *widget_style_get_top_expression(widget_style *style);
expression *widget_style_get_left_expression(widget_style *style);

void widget_style_set_size(widget_style *style, float width, float height);
expression *widget_style_get_width_expression(widget_style *style);
expression *widget_style_get_height_expression(widget_style *style);

void widget_style_set_image(
	widget_style *style, RsvgHandle *h, char *svg_id, int width, int height);
void widget_style_set_click_callback(
	widget_style *style, render_selection_callback callback);

render_object *widget_desktop(const char *id);

render_object *widget_generic(const char *id);

void widget_button_set_text(render_object *object, char *text);

#endif /* __RENDER_WIDGET_H__ */
