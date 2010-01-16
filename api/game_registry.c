#include <stdlib.h>
#include <string.h>
#include "game_registry.h"

game_registry *game_registry_create() {
	game_registry *registry = calloc(1, sizeof(game_registry));
	registry->games = g_hash_table_new(g_str_hash, g_str_equal);
	return registry;
}

void game_registry_free(game_registry *registry) {
	g_hash_table_unref(registry->games);
	free(registry);
}

void game_registry_add(game *game) {

}

game *game_create(const char* name, solitaire_create cb) {
	game *g = calloc(1, sizeof(game));
	g->name = strdup(name);
	g->create_instance = cb;
	return g;
}

void game_free(game *game) {
	free(game->name);
	free(game);
}
