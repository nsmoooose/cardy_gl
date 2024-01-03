#ifndef __GAME_REGISTRY_H__
#define __GAME_REGISTRY_H__

#include <glib.h>
#include "solitaire.h"

typedef struct {
	char *name;
	solitaire_create create_instance;
} game;

typedef struct {
	GHashTable *games;
} game_registry;

game_registry *game_registry_create();
void game_registry_free(game_registry *registry);
void game_registry_add(game_registry *registry, const char *id, game *game);

game *game_create(const char *name, solitaire_create cb);
void game_free(game *game);

#endif /* __GAME_REGISTRY_H__ */
