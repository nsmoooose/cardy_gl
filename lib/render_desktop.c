#include "mygl.h"
#include "render.h"

/* Fun algorithms:

Growing grass.
* Up to 30 sections.
* width is the section no.
* length 25 per section.
* twist between each segment should be random between -2 -> 2. Same is used
for every one there after.
* different shades of green (some randomness)
* a new grass straw every 10 seconds.
* max 50 straws.
 */

void render_object_desktop_render(render_object *object, float delta) {
	glClearColor(0.0f, 0.8f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

render_object *render_object_desktop(mem_context *context) {
	render_object *o = render_object_create(context, "desktop");
	o->render = render_object_desktop_render;
	return o;
}
