#include <stdlib.h>
#include "game_system_remote.h"

static void game_system_get_registry(game_system *system) {
}

static void game_system_close(game_system *system) {
}

game_system *game_system_connect(const char* server, int port,
								 const char *username, const char *password) {
	game_system *system = calloc(1, sizeof(game_system));
	system->get_registry = game_system_get_registry;
	system->close = game_system_close;
	return system;
}
