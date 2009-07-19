#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "lib/card.h"

extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;

void update_camera_pos();

void render_pile(pile* pile);
void render_card(pile* pile, card_proxy* proxy);
void render_solitaire(solitaire* sol);
void render_desktop();

#endif /* __RENDERING_H__ */
