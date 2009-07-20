#include "mouse_handler.h"
#include <stdio.h>

void window_mouse(int button, int state, int x, int y) {
	printf("Button: %d, state: %d, position (%d,%d)\n", button, state, x, y);
}

