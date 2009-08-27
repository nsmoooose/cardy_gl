#ifndef __THEME_H__
#define __THEME_H__

#include "memory.h"

typedef struct {
} theme;

typedef struct {
	char **theme_names;
	int theme_count;
} themes;

themes *theme_list(mem_context *context, char *themes_directory);
theme *theme_load(char *themes_directory, char *name);

#endif /* __THEME_H__ */
