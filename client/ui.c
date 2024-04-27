#include "api/render_widget.h"
#include "client/backgrounds/rotating_triangles.h"
#include "client/render_card_themes.h"
#include "client/render_solitaire.h"
#include "client/render_solved.h"
#include "client/render_topmenu.h"
#include "client/ui.h"
#include "client/ui_mainmenu.h"

static void ui_queue_free_all(render_context *rcontext) {
	render_object_queue_free_children(rcontext, rcontext->object);
}

void ui_scene_main(render_context *rcontext) {
	ui_queue_free_all(rcontext);

	render_object *desktop = widget_desktop("desktop");
	render_object_add_child(rcontext->object, desktop);
	render_object_add_child(desktop, ui_mainmenu());
}

void ui_scene_card_themes(render_context *rcontext) {
	ui_queue_free_all(rcontext);

	render_object *desktop = widget_desktop("desktop");
	render_object_add_child(rcontext->object, desktop);
	render_object_card_themes(desktop);
}

void ui_scene_solitaire(render_context *rcontext,
                        solitaire_create sol_callback) {
	ui_queue_free_all(rcontext);

	/*
	root
	 |- game
	     |- triangles
	     |- solitaire
	     |- desktop
	         |- topmenu
	*/

	render_object *game = render_object_create("game");
	render_object_add_child(rcontext->object, game);
	render_object_add_child(game, render_object_triangles());
	render_object_add_child(game, render_object_solitaire(sol_callback));

	render_object *desktop = widget_desktop("desktop");
	render_object_add_child(game, desktop);
	render_object_add_child(desktop, render_object_topmenu());
}

void ui_scene_solitaire_solved(render_context *rcontext) {
	ui_queue_free_all(rcontext);

	render_object_add_child(rcontext->object, render_object_triangles());
	render_object_add_child(rcontext->object, render_object_solved());
}
