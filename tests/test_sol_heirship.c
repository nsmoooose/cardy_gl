#include <check.h>
#include "../api/solitaires/heirship.h"

START_TEST(test_sol_heirship_init) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_heirship(context, settings);

	ck_assert_msg(sol->visual != 0, "visual member should be set");
	ck_assert_msg(sol->visual->pile_count == (3 * 8 + 1),
	              "incorrect number of piles.");
}
END_TEST

START_TEST(test_sol_heirship_deal) {
	visual_pile *deck;
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_heirship(context, settings);
	int index;

	deck = sol->visual->piles[0];

	ck_assert_msg(deck->action != 0, "No action set on pile.");
	deck->action->execute(deck->action);

	ck_assert_msg(sol->visual->piles[0]->card_count == 0,
	              "No card should remain in deck.");

	/* king piles */
	for (index = 1; index < (1 + 8 * 3); index += 3) {
		ck_assert(sol->visual->piles[index]->card_count > 0);
		ck_assert(sol->visual->piles[index]->cards[0]->card != 0);
		ck_assert_msg(sol->visual->piles[index]->cards[0]->card->value == 13,
		              "Bottom card in pile index: %d wasn't a king.", index);
	}
	/* queen piles */
	for (index = 2; index < (2 + 8 * 3); index += 3) {
		ck_assert(sol->visual->piles[index]->card_count == 1);
		ck_assert(sol->visual->piles[index]->cards[0]->card != 0);
		ck_assert_msg(sol->visual->piles[index]->cards[0]->card->value == 12,
		              "Bottom card in pile index: %d wasn't a queen.", index);
	}
	/* ace piles */
	for (index = 3; index < (3 + 8 * 3); index += 3) {
		ck_assert(sol->visual->piles[index]->card_count == 1);
		ck_assert(sol->visual->piles[index]->cards[0]->card != 0);
		ck_assert_msg(sol->visual->piles[index]->cards[0]->card->value == 1,
		              "Bottom card in pile index: %d wasn't a ace.", index);
	}
}
END_TEST

START_TEST(test_sol_heirship_deal2) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_heirship(context, settings);
	visual_pile *deck;
	int index;

	deck = sol->visual->piles[0];
	deck->action->execute(deck->action);
	deck->action->execute(deck->action);

	for (index = 0; index < sol->visual->pile_count; ++index) {
		if (index == 0) {
			ck_assert_msg(sol->visual->piles[index]->card_count == 104,
			              "All cards should have been moved back to the deck");
		} else {
			ck_assert_msg(sol->visual->piles[index]->card_count == 0,
			              "There shouldn't be any cards here.");
		}
	}
	for (index = 0; index < 52; ++index) {
		ck_assert_msg(sol->visual->piles[0]->cards[index]->card == 0,
		              "Card should be hidden.");
	}
}
END_TEST

void add_sol_heirship_tests(Suite *suite) {
	TCase *c = tcase_create("Sol-Heirship");
	tcase_add_test(c, test_sol_heirship_init);
	tcase_add_test(c, test_sol_heirship_deal);
	tcase_add_test(c, test_sol_heirship_deal2);
	suite_add_tcase(suite, c);
}
