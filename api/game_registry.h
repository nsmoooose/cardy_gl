#pragma once

#include <glib.h>
#include "solitaire.h"

typedef struct {
	char *name;
	bool testing;
	solitaire_create create_instance;
} game;

typedef struct {
	GHashTable *games;
} game_registry;

game_registry *game_registry_create(void);
void game_registry_free(game_registry *registry);
void game_registry_add(game_registry *registry, const char *id, game *game);

game *game_create(const char *name, bool testing, solitaire_create cb);
void game_free(game *game);
