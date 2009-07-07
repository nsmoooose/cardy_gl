#include <stdio.h>
#include "keyboard_handler.h"
#include "gameengine.h"
#include "mygl.h"
#include "solitaire_theidiot.h"

void new_game() {
	if(g_solitaire) {
		g_solitaire->deal(g_solitaire);
		printf("COMMAND: deal\n");
	}
}

void window_key_press(unsigned char key, int x, int y) {
	printf("Keyboard pressed: ");
	int key_modifiers = glutGetModifiers();
	if(key_modifiers & GLUT_ACTIVE_SHIFT) {
		printf("SHIFT+");
	}
	if(key_modifiers & GLUT_ACTIVE_CTRL) {
		printf("CTRL+");
	}
	if(key_modifiers & GLUT_ACTIVE_ALT) {
		printf("ALT+");
	}
	printf("%d\n", key);

	switch(key) {
	case '1':
		if(g_solitaire) {
			/* Free the existing instance before we create a new one. */
			g_solitaire->free(g_solitaire);
		}
		g_solitaire = solitaire_theidiot();
		break;

	case 'n':
	case 'N':
		new_game();
		break;
	};
}

void window_special_key_press(int key, int x, int y) {
	printf("Keyboard pressed: ");
	int key_modifiers = glutGetModifiers();
	if(key_modifiers & GLUT_ACTIVE_SHIFT) {
		printf("SHIFT+");
	}
	if(key_modifiers & GLUT_ACTIVE_CTRL) {
		printf("CTRL+");
	}
	if(key_modifiers & GLUT_ACTIVE_ALT) {
		printf("ALT+");
	}
	printf("%d\n", key);
}
