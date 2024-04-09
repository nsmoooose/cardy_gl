#pragma once

#include "api/render.h"
#include "api/solitaire.h"
#include "api/theme.h"

extern theme *g_theme;

typedef struct {
	/* number of values for the side of the card. Use quads */
	int side_count;
	/* number of values for the face of the card. Use triangles */
	int face_count;

	GLfloat *front_vertexes;
	GLfloat *front_texture_coords;
	GLfloat *front_normals;

	GLfloat *back_vertexes;
	GLfloat *back_texture_coords;
	GLfloat *back_normals;

	GLfloat *side_vertexes;
	GLfloat *side_normals;
} card_geometry;

card_geometry *card_geometry_create(mem_context *context,
                                    visual_settings *settings);

extern const char *render_object_solitaire_id;
render_object *render_object_solitaire(solitaire_create callback);
