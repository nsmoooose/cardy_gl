#ifndef __RENDER_WIDGET_H__
#define __RENDER_WIDGET_H__

#include "render.h"

void widget_style_set_backcolor(
	render_object *object, float red, float green, float blue, float alpha);
void widget_style_set_pos(render_object *object, float left, float top);
void widget_style_set_size(render_object *object, float width, float height);
void widget_style_set_image(render_object *object, RsvgHandle *h, char *svg_id);
void widget_style_set_click_callback(
	render_object *object, render_selection_callback callback);

render_object *widget_desktop(const char *id);

render_object *widget_generic(const char *id);

void widget_button_set_text(render_object *object, char *text);

#endif /* __RENDER_WIDGET_H__ */
