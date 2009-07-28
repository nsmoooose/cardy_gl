#include <check.h>
#include "../lib/solitaire_maltesercross.h"

START_TEST(test_sol_maltesercross_init) {
	solitaire* sol = solitaire_maltesercross();

	ck_assert_msg(sol->visual->pile_count == 20, "There should be 20 piles of cards.");
	ck_assert_msg(sol->visual->piles[0]->card_count == 104, "The deck should hold 2 decks of cards.");
}
END_TEST

START_TEST(test_sol_maltesercross_deal) {
	solitaire* sol = solitaire_maltesercross();

	ck_assert_msg(sol->visual->piles[0]->pile_action != 0, "There should be a pile action to deal new cards.");

	sol->visual->piles[0]->pile_action(sol, sol->visual->piles[0]);
	ck_assert_msg(sol->visual->piles[3]->card_count == 13, "Source pile 1 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[4]->card_count == 13, "Source pile 2 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[5]->card_count == 13, "Source pile 3 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[6]->card_count == 13, "Source pile 4 should have 13 cards.");
}
END_TEST
