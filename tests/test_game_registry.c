#include <check.h>
#include "../api/game_registry.h"

START_TEST(test_game_registry_create) {
	game_registry *r = game_registry_create();
	game *g = game_create("name", 0);
	game_registry_add(r, "id", g);
	ck_assert(g_hash_table_size(r->games) == 1);
	ck_assert(g_hash_table_lookup(r->games, "id") == g);
	game_registry_free(r);
}
END_TEST

void add_game_registry_tests(Suite *suite) {
	TCase *c = tcase_create("Expression");
	tcase_add_test(c, test_game_registry_create);
	suite_add_tcase(suite, c);
}

