#pragma once

#include "game_registry.h"

typedef struct game_system_St {
	game_registry *(*get_registry)(struct game_system_St *system);
	void (*close)(struct game_system_St *system);
	void *handle;
} game_system;

game_system *game_system_local(void);
