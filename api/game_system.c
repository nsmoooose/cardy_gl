#include "game_system.h"

game_system *game_system_local() {
	return 0;
}

game_system *game_system_connect(const char* server, int port,
								 const char *username, const char *password) {
	return 0;
}

void game_system_free(game_system *system) {
}
