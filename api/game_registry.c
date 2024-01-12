#include <stdlib.h>
#include <string.h>
#include "game_registry.h"

game_registry *game_registry_create(void) {
	game_registry *registry = calloc(1, sizeof(game_registry));
	registry->games = g_hash_table_new(g_str_hash, g_str_equal);
	return registry;
}

void game_registry_free(game_registry *registry) {
	GHashTableIter it;
	gpointer key, value;
	g_hash_table_iter_init(&it, registry->games);
	while (g_hash_table_iter_next(&it, &key, &value)) {
		game_free((game *)value);
	}
	g_hash_table_unref(registry->games);
	free(registry);
}

void game_registry_add(game_registry *registry, const char *id, game *game) {
	g_hash_table_insert(registry->games, (char *)id, game);
}

game *game_create(const char *name, bool testing, solitaire_create cb) {
	game *g = calloc(1, sizeof(game));
	g->testing = testing;
	g->name = strdup(name);
	g->create_instance = cb;
	return g;
}

void game_free(game *game) {
	free(game->name);
	free(game);
}
