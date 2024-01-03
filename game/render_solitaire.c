#include <cairo.h>
#include <librsvg/rsvg.h>
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../api/ease.h"
#include "../api/mygl.h"
#include "../api/resource.h"
#include "../api/theme.h"
#include "render_mainmenu.h"
#include "render_solitaire.h"
#include "render_solved.h"

typedef struct {
	mem_context *context;
	solitaire *sol;
	visual_settings *settings;

	float first_frame;
	float time_elapsed;

	card_geometry *card_geometry;

	float camera_zoom;
	float camera_translateX;
	float camera_translateY;
	float perspective_near;
	float perspective_far;
	float perspective_fov;

	card_proxy *selected_card;

	GLfloat ambient_light[4];
	GLfloat diffuse_light[4];
	GLfloat specular_light[4];
	GLfloat light_pos[4];
} render_solitaire_data;

/* Themes are slow to load. Keep it global for now. */
theme *g_theme = 0;

const char *render_object_solitaire_id = "solitaire";

static void do_card_move(solitaire *sol, visual_pile *pile, card_proxy *card,
                         int count) {
	if (!ruleset_move_card(sol->ruleset, sol->visual, pile, card, count)) {
		ruleset_move_individual_card(sol->ruleset, sol->visual, pile, card,
		                             count);
	}
}

static void process_click(render_context *rcontext, render_object *object,
                          render_solitaire_data *data, visual_pile *pile,
                          card_proxy *proxy) {

	int card_count;

	if (pile && pile->action) {
		pile->action->execute(pile->action);
	} else if (pile && proxy) {
		if (data->selected_card == proxy) {
			data->selected_card = 0;
		} else {
			if (data->selected_card == 0) {
				data->selected_card = proxy;
			} else {
				card_count = visual_get_rest_of_pile(data->sol->visual,
				                                     data->selected_card);
				do_card_move(data->sol, pile, data->selected_card, card_count);
				data->selected_card = 0;
			}
		}
	} else if (pile) {
		if (data->selected_card) {
			card_count =
				visual_get_rest_of_pile(data->sol->visual, data->selected_card);
			do_card_move(data->sol, pile, data->selected_card, card_count);
			data->selected_card = 0;
		}
	}

	if (data->sol->ruleset->solved &&
	    rule_check(data->sol->ruleset->solved, 0)) {
		render_object_add_child(object->parent, render_object_solved());
		render_object_free(rcontext, object);
	}
}

static void callback_pile(render_event_args *event, void *data) {
	process_click(event->rcontext, event->object, event->object->data, data, 0);
}

static void callback_card(render_event_args *event, void *data) {
	render_solitaire_data *sol_data = event->object->data;
	visual_pile *pile = visual_find_pile_from_card(sol_data->sol->visual, data);
	process_click(event->rcontext, event->object, event->object->data, pile,
	              data);
}

card_geometry *card_geometry_create(mem_context *context,
                                    visual_settings *settings) {

	card_geometry *geo = mem_alloc(context, sizeof(card_geometry));
	int i = 0, j = 0, side_index = 0, face_index = 0, coord_index = 0;
	float cw = settings->card_width;
	float ch = settings->card_height;
	float hw = settings->card_width / 2.0f;
	float hh = settings->card_height / 2.0f;
	float ht = settings->card_thickness / 2.0f;
	float cr = settings->corner_width;
	float angle, sa;

	/* 4 corners * corner_segments + 4 side quads == quads
	 * quads * 4 == vertexes
	 * vertexes * 3 == data values
	 */
	geo->side_count = (4 * settings->corner_segments + 4) * 4 * 3;
	geo->face_count = 5 * 2 * 4 * settings->corner_segments * 3;

	geo->back_vertexes = mem_alloc(context, geo->face_count * sizeof(GLfloat));
	geo->back_normals = mem_alloc(context, geo->face_count * sizeof(GLfloat));
	geo->back_texture_coords =
		mem_alloc(context, geo->face_count / 3 * 2 * sizeof(GLfloat));

	geo->side_vertexes = mem_alloc(context, geo->side_count * sizeof(GLfloat));
	geo->side_normals = mem_alloc(context, geo->side_count * sizeof(GLfloat));

	geo->front_vertexes = mem_alloc(context, geo->face_count * sizeof(GLfloat));
	geo->front_normals = mem_alloc(context, geo->face_count * sizeof(GLfloat));
	geo->front_texture_coords =
		mem_alloc(context, geo->face_count / 3 * 2 * sizeof(GLfloat));

	/* top quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = -1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = -1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = -1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = -1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;

	/* 1
	   |\
	   | \
	  3|__\2
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = 0.0f;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/*1____2
	   \  |
	    \ |
	     \|3
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = 0.0f;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = 0.0f;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/* left quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = -1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = -1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht / 2.0f;
	geo->side_normals[side_index + 0] = -1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	geo->side_normals[side_index + 0] = -1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;

	/* 1
	   |\
	   | \
	  3|__\2
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 0.0f;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 0.0f;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/*1____2
	   \  |
	    \ |
	     \|3
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 0.0f;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/* right quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = 1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = 1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht / 2.0f;
	geo->side_normals[side_index + 0] = 1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	geo->side_normals[side_index + 0] = 1.0f;
	geo->side_normals[side_index + 1] = 0.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;

	/* 1
	   |\
	   | \
	  3|__\2
	 */
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 1.0f;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/*1____2
	   \  |
	    \ |
	     \|3
	 */
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 1.0f;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 1.0f;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/* bottom quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = 1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = 1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = 1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	geo->side_normals[side_index + 0] = 0.0f;
	geo->side_normals[side_index + 1] = 1.0f;
	geo->side_normals[side_index + 2] = 0.0f;
	side_index += 3;

	/* 1
	   |\
	   | \
	  3|__\2
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = 1.0f;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = 1.0f;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/*1____2
	   \  |
	    \ |
	     \|3
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = 1.0f;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/* middle quad */
	/* 1
	   |\
	   | \
	  3|__\2
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/*1____2
	   \  |
	    \ |
	     \|3
	 */
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	geo->front_normals[face_index + 0] = 0.0f;
	geo->front_normals[face_index + 1] = 0.0f;
	geo->front_normals[face_index + 2] = 1.0f;
	face_index += 3;
	coord_index += 2;

	/* corner */
	angle = 0.0f;
	sa = M_PI / 2 / settings->corner_segments;
	for (i = 0; i < 4; ++i) {
		float w = 0.0, h = 0.0;
		switch (i) {
		case 0:
			/* top left */
			w = hw - cr;
			h = hh - cr;
			break;
		case 1:
			/* bottom left */
			w = hw - cr;
			h = 0.0f - hh + cr;
			break;
		case 2:
			/* bottom right */
			w = 0.0f - hw + cr;
			h = 0.0f - hh + cr;
			break;
		case 3:
			/* top right */
			w = 0.0f - hw + cr;
			h = hh - cr;
			break;
		}

		for (j = 0; j < settings->corner_segments; ++j) {
			geo->side_vertexes[side_index + 0] = 0.0f + w + sinf(angle) * cr;
			geo->side_vertexes[side_index + 1] = 0.0f + h + cosf(angle) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f - ht;
			geo->side_normals[side_index + 0] = 0.0f + sinf(angle);
			geo->side_normals[side_index + 1] = 0.0f + cosf(angle);
			geo->side_normals[side_index + 2] = 0.0f;
			side_index += 3;

			geo->side_vertexes[side_index + 0] =
				0.0f + w + sinf(angle + sa) * cr;
			geo->side_vertexes[side_index + 1] =
				0.0f + h + cosf(angle + sa) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f - ht;
			geo->side_normals[side_index + 0] = 0.0f + sinf(angle);
			geo->side_normals[side_index + 1] = 0.0f + cosf(angle);
			geo->side_normals[side_index + 2] = 0.0f;
			side_index += 3;

			geo->side_vertexes[side_index + 0] =
				0.0f + w + sinf(angle + sa) * cr;
			geo->side_vertexes[side_index + 1] =
				0.0f + h + cosf(angle + sa) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f + ht;
			geo->side_normals[side_index + 0] = 0.0f + sinf(angle);
			geo->side_normals[side_index + 1] = 0.0f + cosf(angle);
			geo->side_normals[side_index + 2] = 0.0f;
			side_index += 3;

			geo->side_vertexes[side_index + 0] = 0.0f + w + sinf(angle) * cr;
			geo->side_vertexes[side_index + 1] = 0.0f + h + cosf(angle) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f + ht;
			geo->side_normals[side_index + 0] = 0.0f + sinf(angle);
			geo->side_normals[side_index + 1] = 0.0f + cosf(angle);
			geo->side_normals[side_index + 2] = 0.0f;
			side_index += 3;

			geo->front_vertexes[face_index + 0] = 0.0f + w + sinf(angle) * cr;
			geo->front_vertexes[face_index + 1] = 0.0f + h + cosf(angle) * cr;
			geo->front_vertexes[face_index + 2] = 0.0f + ht;
			geo->front_texture_coords[coord_index + 0] =
				(geo->front_vertexes[face_index + 0] + hw) / cw;
			geo->front_texture_coords[coord_index + 1] =
				(geo->front_vertexes[face_index + 1] + hh) / ch;
			geo->front_normals[face_index + 0] = 0.0f;
			geo->front_normals[face_index + 1] = 0.0f;
			geo->front_normals[face_index + 2] = 1.0f;
			face_index += 3;
			coord_index += 2;
			geo->front_vertexes[face_index + 0] =
				0.0f + w + sinf(angle + sa) * cr;
			geo->front_vertexes[face_index + 1] =
				0.0f + h + cosf(angle + sa) * cr;
			geo->front_vertexes[face_index + 2] = 0.0f + ht;
			geo->front_texture_coords[coord_index + 0] =
				(geo->front_vertexes[face_index + 0] + hw) / cw;
			geo->front_texture_coords[coord_index + 1] =
				(geo->front_vertexes[face_index + 1] + hh) / ch;
			geo->front_normals[face_index + 0] = 0.0f;
			geo->front_normals[face_index + 1] = 0.0f;
			geo->front_normals[face_index + 2] = 1.0f;
			face_index += 3;
			coord_index += 2;
			geo->front_vertexes[face_index + 0] = w;
			geo->front_vertexes[face_index + 1] = h;
			geo->front_vertexes[face_index + 2] = 0.0f + ht;
			geo->front_texture_coords[coord_index + 0] = (w + hw) / cw;
			geo->front_texture_coords[coord_index + 1] = (h + hh) / ch;
			geo->front_normals[face_index + 0] = 0.0f;
			geo->front_normals[face_index + 1] = 0.0f;
			geo->front_normals[face_index + 2] = 1.0f;
			face_index += 3;
			coord_index += 2;

			angle += sa;
		}
	}

	for (i = 0; i < geo->face_count; ++i) {
		geo->back_vertexes[i] = geo->front_vertexes[i];
		geo->back_normals[i] = geo->front_normals[i];
		if ((i + 1) % 3 == 0) {
			geo->back_vertexes[i] = 0.0f - ht;
			geo->back_normals[i] = -1.0f;
		}
	}
	for (i = 0; i < geo->face_count / 3 * 2; ++i) {
		geo->back_texture_coords[i] = geo->front_texture_coords[i];
	}

	return geo;
}

void render_card(render_event_args *event, visual_pile *pile, card_proxy *proxy,
                 bool selected, card_geometry *geo) {
	GLfloat color_sel[] = {1.0f, 0.7f, 0.7f};
	GLfloat color_white[] = {1.0f, 1.0f, 1.0f};
	render_solitaire_data *i = event->object->data;

	if (selected) {
		glColor3fv(color_sel);
	} else {
		glColor3fv(color_white);
	}

	glPushName(render_register_selection_callback(
		event->rcontext, event->object, callback_card, proxy));

	/* Test to see if we need to rotate the card around its axis
	   to show the front face. */
	glPushMatrix();
	if (proxy->card == 0) {
		/* Rotate the card to show the front face of the card. */
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	}

	/* Draw the side of the card. */
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, geo->side_normals);
	glVertexPointer(3, GL_FLOAT, 0, geo->side_vertexes);
	glDrawArrays(GL_QUADS, 0, geo->side_count / 3);

	/* Front of the card. */
	glNormalPointer(GL_FLOAT, 0, geo->front_normals);
	if (proxy->card != 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,
		              theme_get_card_texture(g_theme, proxy->card->suit,
		                                     proxy->card->value));
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, geo->front_texture_coords);
	}
	glVertexPointer(3, GL_FLOAT, 0, geo->front_vertexes);
	glDrawArrays(GL_TRIANGLES, 0, geo->face_count / 3);
	if (proxy->card != 0) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	/* Back side of the card. */
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glNormalPointer(GL_FLOAT, 0, geo->back_normals);
	glTexCoordPointer(2, GL_FLOAT, 0, geo->back_texture_coords);
	glVertexPointer(3, GL_FLOAT, 0, geo->back_vertexes);
	glBindTexture(GL_TEXTURE_2D, theme_get_card_back_texture(g_theme));
	glDrawArrays(GL_TRIANGLES, 0, geo->face_count / 3);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glPopName();

	/* Do a translation of our position for the next card. */
	glTranslatef(pile->translateX, pile->translateY,
	             i->settings->card_thickness);
	check_gl_errors("render_card");
}

void render_pile(render_event_args *event, visual_pile *pile,
                 visual_settings *settings, card_geometry *geo) {
	int card_index;
	bool selected = false;
	render_solitaire_data *i = event->object->data;

	/* Do a translation of our position for the pile. */
	glPushMatrix();
	glTranslatef(pile->origin[0], pile->origin[1], pile->origin[2]);

	glPushName(render_register_selection_callback(
		event->rcontext, event->object, callback_pile, pile));

	if (pile->rotation != 0.0f) {
		glRotatef(pile->rotation, 0.0f, 0.0f, 1.0f);
	}

	glColor3f(0.0f, 0.7f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(0 - settings->card_width / 2.0f,
	           0 + settings->card_height / 2.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0 + settings->card_width / 2.0f,
	           0 + settings->card_height / 2.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0 + settings->card_width / 2.0f,
	           0 - settings->card_height / 2.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0 - settings->card_width / 2.0f,
	           0 - settings->card_height / 2.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glEnd();

	check_gl_errors("render_pile (1)");

	glPushMatrix();
	for (card_index = 0; card_index < pile->card_count; ++card_index) {
		if (pile->cards[card_index] == i->selected_card) {
			selected = true;
		}
		render_card(event, pile, pile->cards[card_index], selected, geo);
	}
	glPopMatrix();

	glPopMatrix();
	check_gl_errors("render_pile (2)");
	glPopName();
}

void render_object_solitaire_render(render_event_args *event, float delta) {
	int pile_index;
	int pile_count;
	render_solitaire_data *i = event->object->data;
	GLint viewport[4];
	GLfloat aspect;
	render_pick *pick;

	if (i->first_frame == 0.0f) {
		i->first_frame = delta;
		i->time_elapsed = delta;
		return;
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, i->ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, i->diffuse_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, i->specular_light);
	glLightfv(GL_LIGHT0, GL_POSITION, i->light_pos);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);

	glEnableClientState(GL_VERTEX_ARRAY);

	glGetIntegerv(GL_VIEWPORT, viewport);
	if (viewport[3] == 0) {
		viewport[3] = 1;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (event->rcontext->pick) {
		pick = event->rcontext->pick;
		gluPickMatrix(pick->x, pick->y, pick->width, pick->height,
		              pick->viewport);
	}
	aspect = (float)viewport[2] / (float)viewport[3];
	gluPerspective(i->perspective_fov, aspect, i->perspective_near,
	               i->perspective_far);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();

	glTranslatef(i->camera_translateX, i->camera_translateY, i->camera_zoom);
	glRotatef(
		ease_time_protect4f(ease_quad_out, i->time_elapsed, 0.0, -25.0f, 12.0),
		1.0f, 0.0f, 0.0f);

	if (i->time_elapsed < 6.0) {
		float distance =
			ease_quint_out(i->time_elapsed, -2500.0f, 2500.0f, 6.0f);
		float angle = ease_quint_out(i->time_elapsed, 720.0, -720.0f, 6.0f);
		glTranslatef(0, 0, distance);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
	}

	pile_count = i->sol->visual->pile_count;
	for (pile_index = 0; pile_index < pile_count; ++pile_index) {
		visual_pile *pile = i->sol->visual->piles[pile_index];
		if (!pile) {
			continue;
		}
		render_pile(event, pile, i->sol->visual->settings, i->card_geometry);
	}
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	i->time_elapsed += delta;
}

static void render_object_solitaire_change_card_theme(const char *theme) {
	char theme_dir[PATH_MAX];
	if (resource_get_dir(theme_dir, PATH_MAX)) {
		strncat(theme_dir, "themes", PATH_MAX - 1);
		theme_unload(g_theme);
		g_theme = theme_load(theme_dir, theme);
	}
}

static bool render_object_solitaire_keyboard_down(render_event_args *event,
                                                  unsigned char key,
                                                  int modifiers, int x, int y) {
	render_object *object;
	render_solitaire_data *i = event->object->data;

	/*
	int key_modifiers;

	printf("Keyboard pressed: ");
	key_modifiers = glutGetModifiers();
	if(key_modifiers & GLUT_ACTIVE_SHIFT) {
	        printf("SHIFT+");
	}
	if(key_modifiers & GLUT_ACTIVE_CTRL) {
	        printf("CTRL+");
	}
	if(key_modifiers & GLUT_ACTIVE_ALT) {
	        printf("ALT+");
	}
	printf("%d\n", key);
	*/

	switch (key) {
	case 27:
		object = render_object_find(event->rcontext->object,
		                            render_object_mainmenu_id);
		if (object == 0) {
			object = render_object_find(event->rcontext->object, "desktop");
			render_object_mainmenu(object);
		} else {
			render_object_free(event->rcontext, object);
		}
		return true;
	case '-':
		i->camera_zoom -= 10.0f;
		return true;

	case '+':
		i->camera_zoom += 10.0f;
		return true;

	case 'z':
		render_object_solitaire_change_card_theme("ancient_french");
		return true;
	case 'x':
		render_object_solitaire_change_card_theme("anglo");
		return true;
	case 'c':
		render_object_solitaire_change_card_theme("atlasnye");
		return true;
	case 'v':
		render_object_solitaire_change_card_theme("dondorf");
		return true;
	case 'b':
		render_object_solitaire_change_card_theme("paris");
		return true;
	default:
		return false;
	};
}

bool render_object_solitaire_special_down(render_event_args *event, int key,
                                          int modifiers, int x, int y) {
	render_solitaire_data *i = event->object->data;

	switch (key) {
	case 100:
		/* Left */
		i->camera_translateX -= 10.0f;
		return true;
	case 101:
		/* Up */
		i->camera_translateY += 10.0f;
		return true;
	case 102:
		/* Right */
		i->camera_translateX += 10.0f;
		return true;
	case 103:
		/* Down */
		i->camera_translateY -= 10.0f;
		return true;
	default:
		return false;
	}
}

void render_object_solitaire_free(render_event_args *event) {
	render_solitaire_data *i = event->object->data;
	mem_context_free(i->context);
	free(i);
}

render_object *render_object_solitaire(solitaire_create callback) {
	render_solitaire_data *i = calloc(1, sizeof(render_solitaire_data));
	render_object *o = render_object_create(render_object_solitaire_id);
	o->data = i;
	o->render = render_object_solitaire_render;
	o->keyboard_down = render_object_solitaire_keyboard_down;
	o->keyboard_special_down = render_object_solitaire_special_down;
	o->free = render_object_solitaire_free;

	i->context = mem_context_create();
	i->settings = mem_alloc(i->context, sizeof(visual_settings));
	i->settings->corner_width = 2.0f;
	i->settings->corner_segments = 4;
	i->settings->card_width = 40.0f;
	i->settings->card_height = 60.0f;
	i->settings->card_spacing = 4.0f;
	i->settings->card_thickness = 0.4f;

	i->card_geometry = card_geometry_create(i->context, i->settings);

	/* Setup some default values. These could optionally be saved
	 * in some settings file or database.
	 */
	i->camera_zoom = -500.0f;
	i->camera_translateX = 0.0f;
	i->camera_translateY = 0.0f;
	i->perspective_near = 1.0f;
	i->perspective_far = 5000.0f;
	i->perspective_fov = 45.0f;
	i->selected_card = 0;

	i->ambient_light[0] = 0.01f;
	i->ambient_light[1] = 0.01f;
	i->ambient_light[2] = 0.01f;
	i->ambient_light[3] = 1.0f;
	i->diffuse_light[0] = 0.8f;
	i->diffuse_light[1] = 0.8f;
	i->diffuse_light[2] = 0.8f;
	i->diffuse_light[3] = 1.0f;
	i->specular_light[0] = 1.0f;
	i->specular_light[1] = 1.0f;
	i->specular_light[2] = 1.0f;
	i->specular_light[3] = 1.0f;
	i->light_pos[0] = 0.0f;
	i->light_pos[1] = 0.0f;
	i->light_pos[2] = 100.0f;
	i->light_pos[3] = 1.0f;

	if (g_theme == 0) {
		char themes_path[PATH_MAX];
		resource_get_dir(themes_path, PATH_MAX);
		strncat(themes_path, "themes", PATH_MAX - 1);
		g_theme = theme_load(themes_path, "paris");
	}

	i->sol = callback(i->context, i->settings);

	return o;
}
