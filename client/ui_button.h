#pragma once

#include "api/render_widget.h"

render_object *ui_button(const char *id, expression *x, expression *y,
                         const char *text, render_selection_callback callback);
