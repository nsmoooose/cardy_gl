#ifndef __MYGL_H__
#define __MYGL_H__

#ifdef WIN32
#include <GL/glee.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>

void check_gl_errors(char *last_section);

#endif /* __MYGL_H__ */
