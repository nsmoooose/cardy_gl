#include <dirent.h>
#include <string.h>
#include "theme.h"

void theme_add_theme(mem_context *context, themes *themes, char *theme_name) {
	char** old_names;
	int buffer_size = strlen(theme_name) + 1;
	char *buffer = mem_alloc(context, buffer_size);
	strncpy(buffer, theme_name, buffer_size);

	old_names  = themes->theme_names;
	themes->theme_names = mem_alloc(context, (themes->theme_count + 1) * sizeof(char*));
	themes->theme_count++;

	if(old_names) {
		memcpy(themes->theme_names, old_names, sizeof(char*) * (themes->theme_count - 1));
		mem_free(context, old_names);
	}
	themes->theme_names[themes->theme_count - 1] = buffer;
}

themes *theme_list(mem_context *context, char *themes_directory) {
	struct dirent *dp;
	themes *t;
	DIR *dir = opendir(themes_directory);
	if(dir == 0) {
		return 0;
	}

	t = mem_alloc(context, sizeof(themes));
	while ((dp=readdir(dir)) != 0) {
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
			continue;
		}
		theme_add_theme(context, t, dp->d_name);
	}
	closedir(dir);
	return t;
}
