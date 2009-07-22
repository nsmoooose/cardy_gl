#include <check.h>
#include "../lib/solitaire_theidiot.h"

START_TEST(test_sol_theidiot_init) {
	solitaire* sol = solitaire_theidiot();

	ck_assert_msg(sol->new_game != 0, "New game isn't implemented.");
	ck_assert_msg(sol->deal != 0, "Deal new cards isn't implemented.");
	ck_assert_msg(sol->get_pile_count != 0, "Get pile count isn't implemented.");
	ck_assert_msg(sol->get_pile != 0, "Get pile isn't implemented.");
	ck_assert_msg(sol->move != 0, "Move card isn't implemented.");
	ck_assert_msg(sol->card_revealed == 0, "Card revealed callback is set.");
	ck_assert_msg(sol->finished == 0, "Finished callback is set.");
	ck_assert_msg(sol->free != 0, "Free isn't implemented.");
	ck_assert_msg(sol->data != 0, "Private implementation isn't set.");

	ck_assert_msg(sol->get_pile_count(sol) == 6, "Incorrect number of piles.");

	int card_count[] = {52, 0, 0, 0, 0, 0};
	int i;
	for(i=0;i<6;++i) {
		pile* p = sol->get_pile(sol, i);
		ck_assert_msg(p != 0, "Pile was 0");
		ck_assert_msg(p->card_count == card_count[i], "The number of cards wasn't %d. It was: %d on pile: %d", card_count[i], p->card_count, i);
		ck_assert_msg(p->first != 0, "Pile buffer empty");
	}
}
END_TEST

START_TEST(test_sol_theidiot_deal) {
	solitaire* sol = solitaire_theidiot();

	pile* deck = sol->get_pile(sol, 0);
	deck->pile_action(sol, deck);

	ck_assert_msg(deck->card_count == (52 - 4), "The deck should have 4 less cards now.");
}
END_TEST
