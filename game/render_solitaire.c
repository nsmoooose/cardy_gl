#include <cairo.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
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
} render_solitaire_data;

theme *g_theme = 0;

float g_camera_zoom = -500.0f;
float g_camera_translateX = 0.0f;
float g_camera_translateY = 0.0f;
float g_perspective_near = 1.0f;
float g_perspective_far = 5000.0f;
float g_perspective_fov = 45.0f;

card_proxy *g_selected_card = 0;

const char *render_object_solitaire_id = "solitaire";

GLfloat ambient_light[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat diffuse_light[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_pos[] = { 0.0f, 0.0f, 100.0f, 1.0f };

/* Build a vector of coordinates for a card. */
static GLfloat g_card_vertexes[8*3] = {
	0 - CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 0, top left, front */
	0 + CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 1, top right, front */
	0 + CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 2, bottom right, front */
	0 - CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 + CARD_THICKNESS/2.0f, /* 3, bottom left, front */
	0 - CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f, /* 4, top left, back */
	0 + CARD_WIDTH/2.0f, 0 + CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f, /* 5, top right, back */
	0 + CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f, /* 6, bottom right, back */
	0 - CARD_WIDTH/2.0f, 0 - CARD_HEIGHT/2.0f, 0 - CARD_THICKNESS/2.0f /* 7, bottom left, back */
};

static GLubyte g_card_indexes[] = {
	0, 1, 2, 3, /* front face */
	4, 5, 1, 0, /* top */
	3, 2, 6, 7, /* bottom */
	5, 4, 7, 6, /* back face */
	1, 5, 6, 2, /* right */
	4, 0, 3, 7 /* left */
};

static void do_card_move(solitaire *sol, visual_pile *pile,
					card_proxy *card, int count) {
	if(!ruleset_move_card(sol->ruleset, sol->visual, pile, card, count)) {
		ruleset_move_individual_card(sol->ruleset, sol->visual, pile, card, count);
	}
}

static void process_click(
	render_context *rcontext,
	render_object *object, render_solitaire_data *data,
	visual_pile *pile, card_proxy *proxy) {

	int card_count;

	if(pile && pile->action) {
		pile->action->execute(pile->action);
	}
	else if(pile && proxy) {
		if(g_selected_card == proxy) {
			g_selected_card = 0;
		}
		else {
			if(g_selected_card == 0) {
				g_selected_card = proxy;
			}
			else {
				card_count = visual_get_rest_of_pile(
					data->sol->visual, g_selected_card);
				do_card_move(data->sol, pile, g_selected_card, card_count);
				g_selected_card = 0;
			}
		}
	}
	else if(pile) {
		if(g_selected_card) {
			card_count = visual_get_rest_of_pile(
				data->sol->visual, g_selected_card);
			do_card_move(data->sol, pile, g_selected_card, card_count);
			g_selected_card = 0;
		}
	}

	if(data->sol->ruleset->solved && rule_check(data->sol->ruleset->solved, 0)) {
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
	process_click(event->rcontext, event->object, event->object->data, pile, data);
}

card_geometry *card_geometry_create(
	mem_context *context, visual_settings *settings) {

	card_geometry *geo = mem_alloc(context, sizeof(card_geometry));
	int i=0, j=0, side_index=0, face_index=0, coord_index=0;
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
	geo->front_vertexes = mem_alloc(context,  geo->face_count * sizeof(GLfloat));
	geo->back_vertexes = mem_alloc(context, geo->face_count * sizeof(GLfloat));
	geo->side_vertexes = mem_alloc(context, geo->side_count * sizeof(GLfloat));
	geo->front_texture_coords = mem_alloc(context, geo->face_count / 3 * 2 * sizeof(GLfloat));
	geo->back_texture_coords = mem_alloc(context, geo->face_count / 3 * 2 * sizeof(GLfloat));

	/* top quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f - hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = 0.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	face_index += 3;
	coord_index += 2;

	/* left quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht / 2.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 0.0f;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;

	/* right quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f - hh + cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht / 2.0f;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw;
	geo->side_vertexes[side_index + 1] = 0.0f + hh - cr;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 1.0f;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 1.0f;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = 1.0f;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;

	/* bottom quad and side */
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f - ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f + hw - cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
	side_index += 3;
	geo->side_vertexes[side_index + 0] = 0.0f - hw + cr;
	geo->side_vertexes[side_index + 1] = 0.0f + hh;
	geo->side_vertexes[side_index + 2] = 0.0f + ht;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = 1.0f;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = 1.0f;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = 1.0f;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f - hw + cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = cr / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
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
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f + hh - cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = cr / ch;
	face_index += 3;
	coord_index += 2;
	geo->front_vertexes[face_index + 0] = 0.0f + hw - cr;
	geo->front_vertexes[face_index + 1] = 0.0f - hh + cr;
	geo->front_vertexes[face_index + 2] = 0.0f + ht;
	geo->front_texture_coords[coord_index + 0] = (cw - cr) / cw;
	geo->front_texture_coords[coord_index + 1] = (ch - cr) / ch;
	face_index += 3;
	coord_index += 2;

	/* corner */
	angle = 0.0f;
	sa = M_PI / 2 / settings->corner_segments;
	for(i=0;i<4;++i) {
		float w, h;
		switch(i) {
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

		for(j=0;j<settings->corner_segments;++j) {
			geo->side_vertexes[side_index + 0] = 0.0f + w + sinf(angle) * cr;
			geo->side_vertexes[side_index + 1] = 0.0f + h + cosf(angle) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f - ht;
			side_index += 3;

			geo->side_vertexes[side_index + 0] = 0.0f + w + sinf(angle + sa) * cr;
			geo->side_vertexes[side_index + 1] = 0.0f + h + cosf(angle + sa) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f - ht;
			side_index += 3;

			geo->side_vertexes[side_index + 0] = 0.0f + w + sinf(angle + sa) * cr;
			geo->side_vertexes[side_index + 1] = 0.0f + h + cosf(angle + sa) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f + ht;
			side_index += 3;

			geo->side_vertexes[side_index + 0] = 0.0f + w + sinf(angle) * cr;
			geo->side_vertexes[side_index + 1] = 0.0f + h + cosf(angle) * cr;
			geo->side_vertexes[side_index + 2] = 0.0f + ht;
			side_index += 3;
			angle += sa;
		}
	}

	return geo;
}

void render_card(render_event_args *event, visual_pile* pile,
				 card_proxy* proxy, bool selected, card_geometry *geo) {
	int index;
	GLfloat color_sel[] = { 1.0f, 0.7f, 0.7f };
	GLfloat color_white[] = { 1.0f, 1.0f, 1.0f };

	if(selected) {
		glColor3fv(color_sel);
	}
	else {
		glColor3fv(color_white);
	}

	glPushName(render_register_selection_callback(
				   event->rcontext, event->object, callback_card, proxy));

	/* Test to see if we need to rotate the card around its axis
	   to show the front face. */
	glPushMatrix();
	if(proxy->card == 0) {
		/* Rotate the card to show the front face of the card. */
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	}

	/* Draw the side of the card. */
	glVertexPointer(3, GL_FLOAT, 0, geo->side_vertexes);
	glDrawArrays(GL_QUADS, 0, geo->side_count / 3);

	if(proxy->card != 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, theme_get_card_texture(
						  g_theme, proxy->card->suit, proxy->card->value));
	}
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, geo->front_texture_coords);
	glVertexPointer(3, GL_FLOAT, 0, geo->front_vertexes);
	glDrawArrays(GL_TRIANGLES, 0, geo->face_count / 3);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

#if 0
	for(index=0;index<6;++index) {
		/* Only apply texturing on the front and the back of the
		   card. */
		if(index == 0 && proxy->card != 0) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, theme_get_card_texture(
							  g_theme, proxy->card->suit, proxy->card->value));
		}
		if(index == 3) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, theme_get_card_back_texture(g_theme));
		}

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+0] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+0] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+0] * 3 + 2]);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+1] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+1] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+1] * 3 + 2]);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+2] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+2] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+2] * 3 + 2]);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(
			g_card_vertexes[g_card_indexes[index*4+3] * 3 + 0],
			g_card_vertexes[g_card_indexes[index*4+3] * 3 + 1],
			g_card_vertexes[g_card_indexes[index*4+3] * 3 + 2]);
		glEnd();

		if(index == 0 || index == 3) {
			glDisable(GL_TEXTURE_2D);
		}
	}
#endif

	glPopMatrix();
	glPopName();

	/* Do a translation of our position for the next card. */
	glTranslatef(pile->translateX, pile->translateY, CARD_THICKNESS);
	check_gl_errors("render_card");
}

void render_pile(render_event_args *event,
				 visual_pile* pile, visual_settings *settings, card_geometry *geo) {
	int card_index;
	bool selected = false;

	/* Do a translation of our position for the pile. */
	glPushMatrix();
	glTranslatef(pile->origin[0], pile->origin[1], pile->origin[2]);

	glPushName(render_register_selection_callback(
				   event->rcontext, event->object, callback_pile, pile));

	if(pile->rotation != 0.0f) {
		glRotatef(pile->rotation, 0.0f, 0.0f, 1.0f);
	}

	glColor3f(0.0f, 0.7f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(0 - settings->card_width/2.0f, 0 + settings->card_height/2.0f);
	glVertex2f(0 + settings->card_width/2.0f, 0 + settings->card_height/2.0f);
	glVertex2f(0 + settings->card_width/2.0f, 0 - settings->card_height/2.0f);
	glVertex2f(0 - settings->card_width/2.0f, 0 - settings->card_height/2.0f);
	glEnd();

	check_gl_errors("render_pile (1)");

	glPushMatrix();
	for(card_index=0;card_index<pile->card_count;++card_index) {
		if(pile->cards[card_index] == g_selected_card) {
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

	if(i->first_frame == 0.0f) {
		i->first_frame = delta;
		i->time_elapsed = delta;
		return;
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);

	glEnableClientState(GL_VERTEX_ARRAY);

	glGetIntegerv(GL_VIEWPORT, viewport);
	if(viewport[3] == 0) {
		viewport[3] = 1;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(event->rcontext->pick) {
		pick = event->rcontext->pick;
		gluPickMatrix(pick->x, pick->y, pick->width, pick->height, pick->viewport);
	}
	aspect = (float)viewport[2]/(float)viewport[3];
	gluPerspective(g_perspective_fov, aspect, g_perspective_near,
				   g_perspective_far);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();

	glTranslatef(g_camera_translateX, g_camera_translateY, g_camera_zoom);
	glRotatef(
		ease_time_protect4f(ease_quad_out, i->time_elapsed, 0.0, -25.0f, 12.0),
		1.0f, 0.0f, 0.0f);

	if(i->time_elapsed < 6.0) {
		float distance =
			ease_quint_out(i->time_elapsed, -2500.0f, 2500.0f, 6.0f);
		float angle = ease_quint_out(i->time_elapsed, 720.0, -720.0f, 6.0f);
		glTranslatef(0, 0, distance);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
	}

	pile_count = i->sol->visual->pile_count;
	for(pile_index=0;pile_index<pile_count;++pile_index) {
		visual_pile* pile = i->sol->visual->piles[pile_index];
		if(!pile) {
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
	if(resource_get_dir(theme_dir, PATH_MAX)) {
		strncat(theme_dir, "themes", PATH_MAX);
		theme_unload(g_theme);
		g_theme = theme_load(theme_dir, theme);
	}
}

static bool render_object_solitaire_keyboard_down(
	render_event_args *event, unsigned char key, int modifiers, int x, int y) {
	render_object *object;

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

	switch(key) {
	case 27:
		object = render_object_find(event->rcontext->object, render_object_mainmenu_id);
		if(object == 0) {
			object = render_object_find(event->rcontext->object, "desktop");
			render_object_mainmenu(object);
		}
		else {
			render_object_free(event->rcontext, object);
		}
		return true;
	case '-':
		g_camera_zoom -= 10.0f;
		return true;

	case '+':
		g_camera_zoom += 10.0f;
		return true;

	case 'z':
		render_object_solitaire_change_card_theme("anglo");
		return true;
	case 'x':
		render_object_solitaire_change_card_theme("gnome");
		return true;
	case 'c':
		render_object_solitaire_change_card_theme("kde");
		return true;
	case 'v':
		render_object_solitaire_change_card_theme("life_and_smoth");
		return true;
	case 'b':
		render_object_solitaire_change_card_theme("twigs");
		return true;
	default:
		return false;
	};
}

bool render_object_solitaire_special_down(
	render_event_args *event, int key, int modifiers, int x, int y) {

	switch(key) {
	case 100:
		/* Left */
		g_camera_translateX -= 10.0f;
		return true;
	case 101:
		/* Up */
		g_camera_translateY += 10.0f;
		return true;
	case 102:
		/* Right */
		g_camera_translateX += 10.0f;
		return true;
	case 103:
		/* Down */
		g_camera_translateY -= 10.0f;
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
	i->settings->corner_width = 3;
	i->settings->corner_segments = 4;
	i->settings->card_width = 40.0f;
	i->settings->card_height = 60.0f;
	i->settings->card_spacing = 4.0f;
	i->settings->card_thickness = 0.4f;

	i->card_geometry = card_geometry_create(i->context, i->settings);

	if(g_theme == 0) {
		char themes_path[PATH_MAX];
		resource_get_dir(themes_path, PATH_MAX);
		strncat(themes_path, "themes", PATH_MAX);
		g_theme = theme_load(themes_path, "gnome");
	}

	i->sol = callback(i->context, i->settings);

	return o;
}
