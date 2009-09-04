#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "lib/solitaire.h"
#include "lib/theme.h"

extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;
extern float g_perspective_near;
extern float g_perspective_far;
extern float g_perspective_fov;
extern card_proxy *g_selected_card;
extern solitaire *g_solitaire;
extern mem_context *g_context;
extern theme *g_theme;

void render_setup();
void render_window_size_change(int width, int height);

void render_update_camera_pos();

visual_settings *visual_settings_create(mem_context *context);

/** Renders a single card. */
void render_card(visual_pile* pile, card_proxy* proxy);

/** Renders a single pile.
 */
void render_pile(visual_pile* pile, visual_settings *settings);

/** Renders a solitaire game.
 */
void render_solitaire(solitaire* sol);

/** Renders the background of the desktop.
 */
void render_desktop();

/** Renders the entire scene from scratch. This is the main entry
 *  for the rendering code.
 */
void render_scene();

#endif /* __RENDERING_H__ */
