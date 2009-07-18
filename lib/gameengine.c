#include "gameengine.h"

/** Global instance of the one and only solitaire that is currently
 *  running.
 */
solitaire* g_solitaire = 0;
float g_camera_zoom = -250.0f;
float g_camera_translateX = 0.0f;
float g_camera_translateY = 0.0f;

void update_camera_pos() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(g_camera_translateX, g_camera_translateY, g_camera_zoom);
}
