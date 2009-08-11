#include <check.h>
#include "../lib/solitaire_theidiot.h"

START_TEST(test_sol_theidiot_init) {
	int card_count[] = {52, 0, 0, 0, 0, 0};
	int i;

	solitaire* sol = solitaire_theidiot();

	ck_assert_msg(sol->new_game != 0, "New game isn't implemented.");
	ck_assert_msg(sol->move != 0, "Move card isn't implemented.");
	ck_assert_msg(sol->card_revealed == 0, "Card revealed callback is set.");
	ck_assert_msg(sol->finished == 0, "Finished callback is set.");
	ck_assert_msg(sol->free != 0, "Free isn't implemented.");
	ck_assert_msg(sol->data != 0, "Private implementation isn't set.");
	ck_assert_msg(sol->visual != 0, "visual member should be set.");
	ck_assert_msg(sol->visual->piles != 0, "piles member should be set.");
	ck_assert_msg(sol->visual->pile_count == 6, "Incorrect number of piles.");

	for(i=0;i<6;++i) {
		vis_pile* p = sol->visual->piles[i];
		ck_assert_msg(p != 0, "Pile was 0");
		ck_assert_msg(p->card_count == card_count[i], "The number of cards wasn't %d. It was: %d on pile: %d", card_count[i], p->card_count, i);
		ck_assert_msg(p->cards != 0, "Pile buffer empty");
	}
}
END_TEST

START_TEST(test_sol_theidiot_deal) {
	vis_pile *deck, *pile1, *pile2, *pile3, *pile4;
	solitaire* sol = solitaire_theidiot();

	deck = sol->visual->piles[0];
	deck->pile_action(sol, deck);

	pile1 = sol->visual->piles[1];
	pile2 = sol->visual->piles[2];
	pile3 = sol->visual->piles[3];
	pile4 = sol->visual->piles[4];

	ck_assert_msg(deck->card_count == (52 - 4), "The deck should have 4 less cards now.");
	ck_assert_msg(pile1->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile2->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile3->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile4->card_count == 1, "The number of cards should be 1");
	ck_assert_msg(pile1->cards[0]->card != 0, "Card should have been revealed.");
	ck_assert_msg(pile2->cards[0]->card != 0, "Card should have been revealed.");
	ck_assert_msg(pile3->cards[0]->card != 0, "Card should have been revealed.");
	ck_assert_msg(pile4->cards[0]->card != 0, "Card should have been revealed.");
}
END_TEST
