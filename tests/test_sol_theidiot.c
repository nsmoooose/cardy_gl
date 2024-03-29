#include <check.h>
#include "api/solitaires/theidiot.h"

START_TEST(test_sol_theidiot_init) {
	int card_count[] = {52, 0, 0, 0, 0, 0};
	int i;

	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_theidiot(context, settings);

	ck_assert_msg(sol->data != 0, "Private implementation isn't set.");
	ck_assert_msg(sol->visual != 0, "visual member should be set.");
	ck_assert_msg(sol->visual->piles != 0, "piles member should be set.");
	ck_assert_msg(sol->visual->pile_count == 6, "Incorrect number of piles.");

	for (i = 0; i < 6; ++i) {
		visual_pile *p = sol->visual->piles[i];
		ck_assert_msg(p != 0, "Pile was 0");
		ck_assert_msg(p->card_count == card_count[i],
		              "The number of cards wasn't %d. It was: %d on pile: %d",
		              card_count[i], p->card_count, i);
		ck_assert_msg(p->cards != 0, "Pile buffer empty");
	}
}
END_TEST

START_TEST(test_sol_theidiot_deal) {
	visual_pile *deck, *pile1, *pile2, *pile3, *pile4;
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_theidiot(context, settings);

	deck = sol->visual->piles[0];
	deck->action->execute(deck->action);

	pile1 = sol->visual->piles[1];
	pile2 = sol->visual->piles[2];
	pile3 = sol->visual->piles[3];
	pile4 = sol->visual->piles[4];

	ck_assert_msg(deck->card_count == (52 - 4),
	              "The deck should have 4 less cards now.");
	ck_assert_msg(pile1->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile2->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile3->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile4->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile1->cards[0]->card != 0,
	              "Card should have been revealed.");
	ck_assert_msg(pile2->cards[0]->card != 0,
	              "Card should have been revealed.");
	ck_assert_msg(pile3->cards[0]->card != 0,
	              "Card should have been revealed.");
	ck_assert_msg(pile4->cards[0]->card != 0,
	              "Card should have been revealed.");
}
END_TEST

START_TEST(test_sol_theidiot_moving_card) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_theidiot(context, settings);
	card *club_4, *club_5, *club_6, *club_king, *spade_4;
	pile *piles[4];

	club_4 = card_create(context, e_clubs, 4);
	club_5 = card_create(context, e_clubs, 5);
	club_6 = card_create(context, e_clubs, 6);
	club_king = card_create(context, e_clubs, 13);
	spade_4 = card_create(context, e_spades, 4);

	piles[0] = (pile *)sol->visual->piles[1]->data;
	piles[1] = (pile *)sol->visual->piles[2]->data;
	piles[2] = (pile *)sol->visual->piles[3]->data;
	piles[3] = (pile *)sol->visual->piles[4]->data;

	piles[0]->cards[0] = club_4;
	piles[1]->cards[0] = club_5;
	piles[1]->cards[1] = club_6;
	piles[2]->cards[0] = club_king;
	piles[3]->cards[0] = spade_4;

	visual_sync(sol->visual);

	ck_assert_msg(
		ruleset_move_card(sol->ruleset, sol->visual, sol->visual->piles[0],
	                      sol->visual->piles[1]->cards[0], 1) == false,
		"Card shouldn't be movable to the deck.");
	ck_assert_msg(
		ruleset_move_card(sol->ruleset, sol->visual, sol->visual->piles[5],
	                      sol->visual->piles[3]->cards[0], 1) == false,
		"King shouldn't be movable to the done pile.");
	ck_assert_msg(
		ruleset_move_card(sol->ruleset, sol->visual, sol->visual->piles[5],
	                      sol->visual->piles[2]->cards[0], 1) == false,
		"Only top card should be movable");
	ck_assert_msg(ruleset_move_card(sol->ruleset, sol->visual,
	                                sol->visual->piles[5],
	                                sol->visual->piles[1]->cards[0], 1) == true,
	              "Card should be movable to done.");
	ck_assert_msg(sol->visual->piles[5]->card_count == 1,
	              "Card should have been moved.");
	ck_assert_msg(sol->visual->piles[5]->cards[0] != 0,
	              "Card index should have been set.");
}
END_TEST

START_TEST(test_sol_theidiot_solved) {
	mem_context *context = mem_context_create();
	visual_settings *settings = mem_alloc(context, sizeof(visual_settings));
	solitaire *sol = solitaire_theidiot(context, settings);
	pile *piles[4];

	ck_assert_msg(sol->ruleset->solved != 0, "No solved rule assigned.");
	ck_assert_msg(rule_check(sol->ruleset->solved, 0) == false,
	              "Should have returned false");

	piles[0] = (pile *)sol->visual->piles[1]->data;
	piles[1] = (pile *)sol->visual->piles[2]->data;
	piles[2] = (pile *)sol->visual->piles[3]->data;
	piles[3] = (pile *)sol->visual->piles[4]->data;

	piles[0]->cards[0] = card_create(context, e_clubs, 14);
	piles[1]->cards[0] = card_create(context, e_spades, 14);
	piles[2]->cards[0] = card_create(context, e_hearts, 14);
	piles[3]->cards[0] = card_create(context, e_diamonds, 14);

	visual_sync(sol->visual);

	ck_assert_msg(rule_check(sol->ruleset->solved, 0) == true,
	              "Should have returned true");
}
END_TEST

void add_sol_theidiot_tests(Suite *suite) {
	TCase *c = tcase_create("Sol-TheIdiot");
	tcase_add_test(c, test_sol_theidiot_init);
	tcase_add_test(c, test_sol_theidiot_deal);
	tcase_add_test(c, test_sol_theidiot_moving_card);
	tcase_add_test(c, test_sol_theidiot_solved);
	suite_add_tcase(suite, c);
}
