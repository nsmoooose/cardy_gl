#ifndef __GAMEENGINE_H__
#define __GAMEENGINE_H__

#include "card.h"
#include "mygl.h"

extern solitaire* g_solitaire;
extern float g_camera_zoom;
extern float g_camera_translateX;
extern float g_camera_translateY;

void update_camera_pos();

#endif // __GAMEENGINE_H__
