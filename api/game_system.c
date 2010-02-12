#include <stdlib.h>
#include "game_system.h"
#include "solitaires/solitaires.h"

typedef struct {
	game_registry *registry;
} game_system_data;

static game_registry *game_system_local_get_registry(game_system *system) {
	game_system_data *data = (game_system_data*)system->handle;
	return data->registry;
}

static void game_system_local_close(game_system *system) {
	game_system_data *data = (game_system_data*)system->handle;
	game_registry_free(data->registry);
	free(system->handle);
	free(system);
}

/* TODO: Move this since code has a dependency to solitaires lib */
game_system *game_system_local() {
	game_system *system = calloc(1, sizeof(game_system));
	game_system_data *data = calloc(1, sizeof(game_system_data));
	system->get_registry = game_system_local_get_registry;
	system->close = game_system_local_close;
	data->registry = solitaire_get_registry();
	system->handle = data;
	return system;
}
