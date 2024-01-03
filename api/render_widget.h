#ifndef __RENDER_WIDGET_H__
#define __RENDER_WIDGET_H__

#include "expression.h"
#include "render.h"

#define style_key_render_time "render_time"
#define style_key_width "width"
#define style_key_height "height"
#define style_key_top "top"
#define style_key_left "left"
#define style_key_backcolor_red "backcolor_red"
#define style_key_backcolor_green "backcolor_green"
#define style_key_backcolor_blue "backcolor_blue"
#define style_key_backcolor_alpha "backcolor_alpha"
#define style_key_viewport_width "viewport_width"
#define style_key_viewport_height "viewport_height"
#define style_key_rotation "rotation"

render_object *widget_desktop(const char *id);
render_object *widget_generic(const char *id);

void widget_set_data(render_object *widget, void *data);
void *widget_get_data(render_object *widget);

struct widget_style_St;
typedef struct widget_style_St widget_style;

typedef enum {
	e_entering_state,
	e_continuous_state,
	e_leaving_state
} widget_transition_state;

/* Enqueu a state change. */
void widget_add_state(render_object *widget, const char *state);

/* Set the leaving state operation for a widget. */
void widget_remove_state(render_object *widget, const char *state);

/* Returns true if the widget or any subwidget has the state
   active in any transition. */
bool widget_state_active(render_object *widget, const char *state);

typedef struct {
	widget_transition_state state;
	float length;
	expression *exp;
} widget_style_transition;

widget_style_transition *widget_transition_create(widget_transition_state state,
                                                  float length,
                                                  expression *exp);

void widget_style_add_transition(widget_style *style, const char *state,
                                 const char *property,
                                 widget_style_transition *t);

widget_style *widget_get_default_style(render_object *object);

expression_context *widget_style_get_expression_context(widget_style *style);

void widget_style_set_backcolor(widget_style *style, float red, float green,
                                float blue, float alpha);

void widget_style_set_pos(widget_style *style, float left, float top);
void widget_style_set_left(widget_style *style, const char *expr);
void widget_style_set_top(widget_style *style, const char *expr);
expression *widget_style_get_top_expression(widget_style *style);
expression *widget_style_get_left_expression(widget_style *style);

void widget_style_set_size(widget_style *style, float width, float height);
void widget_style_set_width(widget_style *style, const char *expr);
void widget_style_set_height(widget_style *style, const char *expr);
expression *widget_style_get_width_expression(widget_style *style);
expression *widget_style_get_height_expression(widget_style *style);

void widget_style_set_rotation(widget_style *style, const char *expr);
expression *widget_style_get_rotation_expression(widget_style *style);

void widget_style_set_image(widget_style *style, RsvgHandle *h, char *svg_id,
                            int width, int height);
void widget_style_set_click_callback(widget_style *style,
                                     render_selection_callback callback);

void widget_style_set_image_size(widget_style *style, int width, int height);
void widget_style_set_font_face(widget_style *style, const char *name);
void widget_style_set_font_size(widget_style *style, float size);
void widget_style_set_text(widget_style *style, const char *text);
void widget_style_set_text_color(widget_style *style, float red, float green,
                                 float blue, float alpha);

#endif /* __RENDER_WIDGET_H__ */
