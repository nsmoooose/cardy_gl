#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <stdbool.h>

bool resource_get_dir(char *buffer, int buffer_length);
bool resource_locate_file(const char *file, char *buffer, int buffer_length);

#endif /* __RESOURCE_H__ */
