#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "api/resource.h"

bool resource_get_dir(char *buffer, int buffer_length) {
	struct stat s;

	/* First we look in our current local directory. */
	strncpy(buffer, "resources", buffer_length);
	if (stat(buffer, &s) == 0) {
		if (getcwd(buffer, buffer_length) == 0) {
			return false;
		}
		strncat(buffer, "/", buffer_length);
		return true;
	}

	/* No this failed. Lets try the global one. */
	strncpy(buffer, "/usr/share/cardy_gl/", buffer_length);
	if (stat(buffer, &s) == -1) {
		return false;
	}
	return true;
}

bool resource_locate_file(const char *file, char *buffer, int buffer_length) {
	if (resource_get_dir(buffer, buffer_length)) {
		strncat(buffer, file, buffer_length);
		return true;
	}
	return false;
}
