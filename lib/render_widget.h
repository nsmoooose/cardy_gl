#ifndef __RENDER_WIDGET_H__
#define __RENDER_WIDGET_H__

#include "render.h"

typedef struct {
	int dummy;
} widget_style;

void widget_style_set_pos(render_object *object, float left, float top);
void widget_style_set_size(render_object *object, float width, float height);
void widget_style_set_image(render_object *object, RsvgHandle *h, char *svg_id);

render_object *widget_desktop(char *id);

render_object *widget_window(char *id);

render_object *widget_button(char *id, char *text);
void widget_button_set_text(render_object *object, char *text);
void widget_button_set_callback(render_object *object, render_selection_callback callback);

#endif /* __RENDER_WIDGET_H__ */
