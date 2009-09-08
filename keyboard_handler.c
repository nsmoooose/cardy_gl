#include <stdio.h>
#include "keyboard_handler.h"
#include "program.h"
#include "rendering.h"
#include "lib/mygl.h"
#include "lib/render_solitaire.h"

void window_key_press(unsigned char key, int x, int y) {
	/*
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
	*/

	switch(key) {
	case '-':
		g_camera_zoom -= 10.0f;
		render_update_camera_pos();
		break;

	case '+':
		g_camera_zoom += 10.0f;
		render_update_camera_pos();
		break;

	case 'z':
		theme_unload(g_theme);
		g_theme = theme_load("themes", "anglo");
		break;
	case 'x':
		theme_unload(g_theme);
		g_theme = theme_load("themes", "gnome");
		break;
	case 'c':
		theme_unload(g_theme);
		g_theme = theme_load("themes", "kde");
		break;
	case 'v':
		theme_unload(g_theme);
		g_theme = theme_load("themes", "life_and_smoth");
		break;
	case 'b':
		theme_unload(g_theme);
		g_theme = theme_load("themes", "twigs");
		break;
	};
	glutPostRedisplay();
}

void window_special_key_press(int key, int x, int y) {
	/*
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
	*/

	switch(key) {
	case 100:
		/* Left */
		g_camera_translateX -= 10.0f;
		render_update_camera_pos();
		break;
	case 101:
		/* Up */
		g_camera_translateY += 10.0f;
		render_update_camera_pos();
		break;
	case 102:
		/* Right */
		g_camera_translateX += 10.0f;
		render_update_camera_pos();
		break;
	case 103:
		/* Down */
		g_camera_translateY -= 10.0f;
		render_update_camera_pos();
		break;
	}
	glutPostRedisplay();
}
