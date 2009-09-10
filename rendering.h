#ifndef __RENDERING_H__
#define __RENDERING_H__

#include "lib/solitaire.h"
#include "lib/theme.h"

void rendering_setup();
void rendering_window_size_change(int width, int height);

/** Renders the entire scene from scratch. This is the main entry
 *  for the rendering code.
 */
void rendering_scene();

void rendering_idle();

#endif /* __RENDERING_H__ */
