#include <check.h>
#include "../lib/solitaire_theidiot.h"

START_TEST(test_sol_theidiot_init) {
	int card_count[] = {52, 0, 0, 0, 0, 0};
	int i;

	solitaire* sol = solitaire_theidiot();

	ck_assert_msg(sol->new_game != 0, "New game isn't implemented.");
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

START_TEST(test_sol_theidiot_moving_card) {
	solitaire* sol = solitaire_theidiot();
	card *club_4, *club_5, *club_king, *spade_4;
	pile *piles[4];

	club_4 = card_create(e_clubs, 4);
	club_5 = card_create(e_clubs, 5);
	club_king = card_create(e_clubs, 13);
	spade_4 = card_create(e_spades, 4);

	piles[0] = (pile*)sol->visual->piles[1]->data;
	piles[1] = (pile*)sol->visual->piles[2]->data;
	piles[2] = (pile*)sol->visual->piles[3]->data;
	piles[3] = (pile*)sol->visual->piles[4]->data;

	piles[0]->cards[0] = club_4;
	piles[1]->cards[0] = club_5;
	piles[2]->cards[0] = club_king;
	piles[3]->cards[0] = spade_4;

	visual_sync(sol->visual);

	ck_assert_msg(sol->append_to_pile != 0, "No append functionality implemented.");
	ck_assert_msg(sol->append_to_pile(sol, sol->visual->piles[5], sol->visual->piles[1]->cards[0]) == true, "Card should be movable.");
}
END_TEST

START_TEST(test_get_move_action) {
	pile *deck, *done;
	visual *vis;
	move_action *action;

	deck = pile_create(52);
	done = pile_create(52);
	create_deck(deck);

	vis = visual_create();
	visual_add_pile(vis, vis_pile_create(deck));
	visual_add_pile(vis, vis_pile_create(done));
	visual_sync(vis);

	action = get_move_action(vis, vis->piles[0]->cards[4], vis->piles[1]);

	ck_assert_msg(action != 0, "No action returned.");
	ck_assert_msg(action->source == deck, "Deck should be the source.");
	ck_assert_msg(action->source_index == 4, "Source index incorrect.");
	ck_assert_msg(action->source_count == 1, "Source count incorrect.");
	ck_assert_msg(action->destination == done, "Done should be destination.");
	ck_assert_msg(action->destination_index == 0, "Destination index incorrect.");
}
END_TEST
