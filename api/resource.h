#pragma once

#include <stdbool.h>

bool resource_get_dir(char *buffer, int buffer_length);
bool resource_locate_file(const char *file, char *buffer, int buffer_length);
