#ifndef __RENDER_SOLITAIRE_H__
#define __RENDER_SOLITAIRE_H__

#include "../api/render.h"
#include "../api/solitaire.h"
#include "../api/theme.h"

extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;
extern float g_perspective_near;
extern float g_perspective_far;
extern float g_perspective_fov;
extern card_proxy *g_selected_card;
extern solitaire *g_solitaire;
extern theme *g_theme;
extern visual_settings *g_visual_settings;

typedef struct {
	/* number of values for the side of the card. Use quads */
	int side_count;
	/* number of values for the face of the card. Use triangles */
	int face_count;

	GLfloat *front_vertexes;
	GLfloat *front_texture_coords;
	GLfloat *back_vertexes;
	GLfloat *back_texture_coords;
	GLfloat *side_vertexes;
} card_geometry;

card_geometry *card_geometry_create(mem_context *context, visual_settings *settings);

extern const char *render_object_solitaire_id;
render_object *render_object_solitaire(solitaire_create callback);

#endif /* __RENDER_SOLITAIRE_H__ */
