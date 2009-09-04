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
	visual_pile_action *action = sol->visual->piles[0]->action;
	int pile_usage = 5;
	int a, b;

	ck_assert_msg(action != 0, "There should be a pile action to deal new cards.");

	action->execute(action);
	ck_assert_msg(sol->visual->piles[7]->card_count == 13, "Source pile 1 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[8]->card_count == 13, "Source pile 2 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[9]->card_count == 13, "Source pile 3 should have 13 cards.");
	ck_assert_msg(sol->visual->piles[10]->card_count == 13, "Source pile 4 should have 13 cards.");

	while(sol->visual->piles[15]->card_count != 52) {
		action->execute(action);
		for(a=0;a<5;++a) {
			if(a < pile_usage) {
				ck_assert_msg(sol->visual->piles[15 + a]->card_count > 0, "source pile should have more than 0 cards.");
			}
			else {
				ck_assert_msg(sol->visual->piles[15 + a]->card_count == 0, "source pile should have 0 cards.");
			}
		}

		/* When the deck is empty the next action will fetch back all cards. */
		if(sol->visual->piles[0]->card_count == 0) {
			pile_usage--;
			/* Execute action to fetch all cards back to the deck. */
			action->execute(action);
			if(pile_usage == 0) {
				ck_assert_msg(sol->visual->piles[15]->card_count == 52, "No fetch back cards to the pile when game is finished.");
			}
			else {
				for(a=0;a<5;++a) {
					ck_assert_msg(sol->visual->piles[15 + a]->card_count == 0, "source pile should have 0 cards when fetching back cards.");
				}
			}
		}

		/* Verify that cards in the deck is hidden. */
		for(b=0;b<sol->visual->piles[0]->card_count;++b) {
			ck_assert_msg(sol->visual->piles[0]->cards[b]->card == 0, "Card should be hidden.");
		}
		/* Verify that the source cards are revealed. */
		for(a=0;a<5;++a) {
			for(b=0;b<sol->visual->piles[15 + a]->card_count;++b) {
				ck_assert_msg(sol->visual->piles[15 + a]->cards[b]->card != 0, "Card should be hidden.");
			}
		}
	}
}
END_TEST

void add_sol_malteser_tests(Suite *suite) {
	TCase *sol_malteser = tcase_create("Sol-MalteserCross");
	tcase_add_test(sol_malteser, test_sol_maltesercross_init);
	tcase_add_test(sol_malteser, test_sol_maltesercross_deal);
	suite_add_tcase(suite, sol_malteser);
}
