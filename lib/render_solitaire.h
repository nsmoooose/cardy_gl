#ifndef __RENDER_SOLITAIRE_H__
#define __RENDER_SOLITAIRE_H__

#include "render.h"
#include "solitaire.h"
#include "theme.h"

extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;
extern float g_perspective_near;
extern float g_perspective_far;
extern float g_perspective_fov;
extern card_proxy *g_selected_card;
extern solitaire *g_solitaire;
extern theme *g_theme;

void render_update_camera_pos();
render_object *render_object_solitaire(mem_context *context);

#endif /* __RENDER_SOLITAIRE_H__ */
