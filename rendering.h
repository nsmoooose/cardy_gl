#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "lib/solitaire.h"
#include "lib/theme.h"

void render_setup();
void render_window_size_change(int width, int height);

/** Renders the entire scene from scratch. This is the main entry
 *  for the rendering code.
 */
void render_scene();

#endif /* __RENDERING_H__ */
