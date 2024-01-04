#include <check.h>
#include "api/game_system.h"

START_TEST(test_game_system_local) {
	game_system *system = game_system_local();

	ck_assert(system != 0);
	ck_assert(system->get_registry != 0);
	ck_assert(system->close != 0);

	system->close(system);
}
END_TEST

START_TEST(test_game_system_registry) {
	game_system *system = game_system_local();
	game_registry *registry = system->get_registry(system);

	ck_assert(registry != 0);
}
END_TEST

void add_game_system_tests(Suite *suite) {
	TCase *c = tcase_create("Card");
	tcase_add_test(c, test_game_system_local);
	tcase_add_test(c, test_game_system_registry);
	suite_add_tcase(suite, c);
}
