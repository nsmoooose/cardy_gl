#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "lib/card.h"

extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;
extern float g_perspective_near;
extern float g_perspective_far;
extern float g_perspective_fov;
extern card_proxy *g_selected_card;
extern solitaire *g_solitaire;
extern mem_context *g_context;

void setup_render_resources();
void check_gl_errors(char *last_section);

void update_camera_pos();

visual_settings *visual_settings_create(mem_context *context);

void render_pile(visual_pile* pile, visual_settings *settings);
void render_card(visual_pile* pile, card_proxy* proxy);
void render_solitaire(solitaire* sol);
void render_desktop();
void render_scene();

#endif /* __RENDERING_H__ */
