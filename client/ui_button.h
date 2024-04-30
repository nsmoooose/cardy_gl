#pragma once

#include "api/render_widget.h"

render_object *ui_button(const char *id, const char *x, const char *y,
                         const char *text, render_selection_callback callback);
