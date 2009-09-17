#include "render_solved.h"

const char *render_object_solved_id = "solitaire_solved";

void render_object_solved_render(render_event_args *event, float delta) {
}

render_object *render_object_solved() {
	render_object *o = render_object_create(render_object_solved_id);
	o->render = render_object_solved_render;
	return o;
}
