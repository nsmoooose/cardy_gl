#include <stdio.h>
#include "keyboard_handler.h"
#include "rendering.h"
#include "lib/mygl.h"
#include "lib/solitaire_theidiot.h"
#include "lib/solitaire_maltesercross.h"
#include "lib/solitaire_noname1.h"

void window_key_press(unsigned char key, int x, int y) {
	int key_modifiers;

	printf("Keyboard pressed: ");
	key_modifiers = glutGetModifiers();
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
	case '2':
		if(g_solitaire) {
			/* Free the existing instance before we create a new one. */
			g_solitaire->free(g_solitaire);
		}
		g_solitaire = solitaire_maltesercross();
		break;
	case '3':
		if(g_solitaire) {
			/* Free the existing instance before we create a new one. */
			g_solitaire->free(g_solitaire);
		}
		g_solitaire = solitaire_noname1();
		break;
	case '-':
		g_camera_zoom -= 10.0f;
		update_camera_pos();
		break;

	case '+':
		g_camera_zoom += 10.0f;
		update_camera_pos();
		break;
	};
	glutPostRedisplay();
}

void window_special_key_press(int key, int x, int y) {
	int key_modifiers = glutGetModifiers();
	printf("Keyboard pressed: ");
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
