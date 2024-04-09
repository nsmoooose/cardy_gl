#pragma once
#include "api/solitaire.h"
#include "api/theme.h"

void rendering_setup(void);
void rendering_window_size_change(int width, int height);

/** Renders the entire scene from scratch. This is the main entry
 *  for the rendering code.
 */
void rendering_scene(void);

void rendering_timer(int value);
