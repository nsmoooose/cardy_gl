#include "render_widget.h"

void widget_style_set_pos(render_object *object, float left, float top) {
}

void widget_style_set_size(render_object *object, float width, float height) {
}

void widget_style_set_image(render_object *object, RsvgHandle *h, char *svg_id) {
}

static void widget_desktop_render(render_event_args *event, float delta) {
}

render_object *widget_desktop(char *id) {
	render_object *o = render_object_create(id);
	o->render = widget_desktop_render;
	return o;
}

render_object *widget_window(char *id) {
	render_object *o = render_object_create(id);
	return o;
}

render_object *widget_button(char *id, char *text) {
	return 0;
}

void widget_button_set_text(render_object *object, char *text) {
}

void widget_button_set_callback(render_object *object, render_selection_callback callback) {
}
