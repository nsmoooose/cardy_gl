#include <check.h>
#include "../lib/solitaire_maltesercross.h"

START_TEST(test_sol_maltesercross_init) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire* sol = solitaire_maltesercross(context, settings);

	ck_assert_msg(sol->visual->pile_count == 20, "There should be 20 piles of cards.");
	ck_assert_msg(sol->visual->piles[0]->card_count == 104, "The deck should hold 2 decks of cards.");
}
END_TEST

START_TEST(test_sol_maltesercross_deal) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire* sol = solitaire_maltesercross(context, settings);

	ck_assert_msg(sol->visual->piles[0]->pile_action != 0, "There should be a pile action to deal new cards.");

	sol->visual->piles[0]->pile_action(sol, sol->visual->piles[0]);
	ck_assert_msg(sol->visual->piles[7]->card_count == 13, "Source pile 1 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[8]->card_count == 13, "Source pile 2 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[9]->card_count == 13, "Source pile 3 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[10]->card_count == 13, "Source pile 4 should have 13 cards.");
}
END_TEST

void add_sol_malteser_tests(Suite *suite) {
	TCase *sol_malteser = tcase_create("Sol-MalteserCross");
	tcase_add_test(sol_malteser, test_sol_maltesercross_init);
	tcase_add_test(sol_malteser, test_sol_maltesercross_deal);
	suite_add_tcase(suite, sol_malteser);
}
