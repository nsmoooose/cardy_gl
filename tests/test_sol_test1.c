#include <check.h>
#include "../game/solitaire_test1.h"

START_TEST(test_sol_test1_solved) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_test1(context, settings);

	ck_assert(sol->visual->pile_count == 2);
	ck_assert(sol->visual->piles[0]->card_count == 52);

	ck_assert(
		ruleset_move_card(
			sol->ruleset, sol->visual, sol->visual->piles[1],
			sol->visual->piles[0]->cards[51], 1) == true);

	ck_assert(rule_check(sol->ruleset->solved, 0) == true);
}
END_TEST

void add_sol_test1_tests(Suite *suite) {
	TCase *c = tcase_create("Sol-Test1");
	tcase_add_test(c, test_sol_test1_solved);
	suite_add_tcase(suite, c);
}
