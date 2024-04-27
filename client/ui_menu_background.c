#include "api/resource.h"
#include "client/ui_menu_background.h"

render_object *ui_menu_background(void) {
	char file_buffer[PATH_MAX];
	resource_locate_file("resources/images/mainmenu.svg", file_buffer,
	                     PATH_MAX);
	RsvgHandle *h = render_svg_open(file_buffer);

	render_object *window = widget_generic(0);
	widget_style *style = widget_get_default_style(window);
	widget_style_set_left(style, "0");
	widget_style_set_top(style, "0");
	widget_style_set_width(style, "viewport_width");
	widget_style_set_height(style, "viewport_height");

	render_object *logo = widget_generic(0);
	style = widget_get_default_style(logo);
	widget_style_set_pos(style, 0.0f, 0.0f);
	widget_style_set_size(style, 342.0f, 138.0f);
	widget_style_set_image(style, h, "#logo", 256, 128);
	render_object_add_child(window, logo);

	render_object *background = widget_generic(0);
	style = widget_get_default_style(background);
	widget_style_set_left(style, "viewport_width/2-width/2");
	widget_style_set_top(style, "viewport_height/2-height/2");
	widget_style_set_width(style, "viewport_height/1.5");
	widget_style_set_height(style, "viewport_height/1.5");
	widget_style_set_image(style, h, "#background", 512, 512);
	widget_style_set_rotation(style, style_key_render_time);
	render_object_add_child(window, background);

	render_svg_close(h);

	return window;
}