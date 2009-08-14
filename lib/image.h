#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "mygl.h"

GLbyte *loadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat);

#endif /* __IMAGE_H__ */
