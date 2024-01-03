#include <stdio.h>
#include "mygl.h"

void check_gl_errors(char *last_section) {
	GLenum errCode;
	const GLubyte *errString;

	/* Print errors if there are any. */
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf(stderr, "OpenGL Error in section: %s, %s\n", last_section,
		        errString);
	}
}
