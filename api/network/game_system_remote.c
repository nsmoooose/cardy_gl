#include <stdlib.h>
#include "game_system_remote.h"

typedef struct {
	int dummy;
} game_system_data;

static game_registry *game_system_get_registry(game_system *system) {
	return 0;
}

static void game_system_close(game_system *system) {
	free(system->handle);
	free(system);
}

game_system *game_system_connect(const char* server, int port,
								 const char *username, const char *password) {
	game_system *system = calloc(1, sizeof(game_system));
	game_system_data *data = calloc(1, sizeof(game_system_data));
	system->get_registry = game_system_get_registry;
	system->close = game_system_close;
	system->handle = data;
	return system;
}
