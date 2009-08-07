#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "lib/card.h"

extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;
extern float g_perspective_near;
extern float g_perspective_far;
extern float g_perspective_fov;

void update_camera_pos();

void render_pile(vis_pile* pile);
void render_card(vis_pile* pile, card_proxy* proxy);
void render_solitaire(solitaire* sol);
void render_desktop();
void render_scene();

#endif /* __RENDERING_H__ */
