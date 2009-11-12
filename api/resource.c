#include <string.h>
#include "resource.h"

bool resource_get_dir(char *buffer, int buffer_length) {
	return false;
}

bool resource_locate_file(const char* file, char *buffer, int buffer_length) {
	if(resource_get_dir(buffer, buffer_length)) {
		strncat(buffer, file, buffer_length);
		return true;
	}
	return false;
}
