#pragma once

#include "api/render.h"
#include "api/solitaire.h"

void ui_scene_main(render_context *rcontext);
void ui_scene_solitaire(render_context *rcontext,
                        solitaire_create sol_callback);
void ui_scene_solitaire_solved(render_context *rcontext);
