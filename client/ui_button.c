#include "client/ui_button.h"

render_object *ui_button(const char *id, const char *x, const char *y,
                         const char *text, render_selection_callback callback) {
	render_object *button = widget_generic(id);
	widget_style *style = widget_get_default_style(button);
	widget_style_set_image_size(style, 128, 32);
	/* widget_style_set_font_face(style, "Meera"); */
	widget_style_set_font_size(style, 16.0f);
	widget_style_set_text_color(style, 0.0f, 0.0f, 0.0f, 1.0f);
	widget_style_set_text(style, text);
	widget_style_set_size(style, 128.0f, 32.0f);
	widget_style_set_left(style, x);
	widget_style_set_top(style, y);
	widget_style_set_click_callback(style, callback);
	return button;
}
