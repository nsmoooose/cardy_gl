#include <stdio.h>
#include "keyboard_handler.h"
#include "lib/gameengine.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"

void new_game() {
	if(g_solitaire) {
		g_solitaire->deal(g_solitaire);
		printf("COMMAND: deal\n");
	}
}

void window_key_press(unsigned char key, int x, int y) {
	static float zoom = -250.0f;
	static float translateX = 0.0f;
	static float translateY = 0.0f;

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
	case '-':
		g_camera_zoom -= 10.0f;
		update_camera_pos();
		break;

	case '+':
		g_camera_zoom += 10.0f;
		update_camera_pos();
		break;

	case 'n':
	case 'N':
		new_game();
		break;
	};
	glutPostRedisplay();
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

	switch(key) {
	case 100:
		/* Left */
		g_camera_translateX -= 10.0f;
		update_camera_pos();
		break;
	case 101:
		/* Up */
		g_camera_translateY += 10.0f;
		update_camera_pos();
		break;
	case 102:
		/* Right */
		g_camera_translateX += 10.0f;
		update_camera_pos();
		break;
	case 103:
		/* Down */
		g_camera_translateY -= 10.0f;
		update_camera_pos();
		break;
	}
	glutPostRedisplay();
}
