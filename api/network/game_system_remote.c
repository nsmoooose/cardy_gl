#include <stdlib.h>
#include <string.h>
#include "game_system_remote.h"

typedef struct {
	game_system_provider *provider;
} game_system_data;

static game_registry *game_system_get_registry(game_system *system) {
	return 0;
}

static void game_system_close(game_system *system) {
	free(system->handle);
	free(system);
}

game_system *game_system_connect(game_system_provider *provider, const char *path, const char *username, const char *password) {
	char buffer[1000];
	int bytes_read;

	game_system *system = calloc(1, sizeof(game_system));
	game_system_data *data = calloc(1, sizeof(game_system_data));
	data->provider = provider;
	system->get_registry = game_system_get_registry;
	system->close = game_system_close;
	system->handle = data;

	data->provider->open(data->provider, path);

	strncpy(buffer, username, 1000);
	strncat(buffer, "\n", 999);
	data->provider->write(data->provider, buffer, strlen(buffer));

	strncpy(buffer, password, 1000);
	strncat(buffer, "\n", 999);
	data->provider->write(data->provider, buffer, strlen(buffer));

	bytes_read = data->provider->read(data->provider, buffer, sizeof(buffer));
	if(bytes_read == 2 && strcmp(buffer, "OK") == 0) {
		return system;
	}
	return 0;
}
