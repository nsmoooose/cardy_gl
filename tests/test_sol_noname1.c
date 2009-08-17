#include <check.h>
#include "../lib/solitaire_noname1.h"

START_TEST(test_sol_noname1_init) {
	solitaire *sol = solitaire_noname1();

	ck_assert_msg(sol->visual != 0, "visual member should be set");
	ck_assert_msg(sol->visual->pile_count == 13, "incorrect number of piles.");
}
END_TEST

START_TEST(test_sol_noname1_deal) {
	vis_pile *deck;
	solitaire *sol = solitaire_noname1();
	int index, index2;

	deck = sol->visual->piles[0];

	ck_assert_msg(deck->pile_action != 0, "No action set on pile.");
	deck->pile_action(sol, deck);

	ck_assert_msg(sol->visual->piles[0]->card_count == 0, "No card should remain in deck.");

	/* Ace piles */
	ck_assert_msg(sol->visual->piles[1]->card_count == 0, "No card should be in ace 1 pile.");
	ck_assert_msg(sol->visual->piles[2]->card_count == 0, "No card should be in ace 2 pile.");
	ck_assert_msg(sol->visual->piles[3]->card_count == 0, "No card should be in ace 3 pile.");
	ck_assert_msg(sol->visual->piles[4]->card_count == 0, "No card should be in ace 4 pile.");

	ck_assert_msg(sol->visual->piles[5]->card_count == 7, "Incorrect number of cards in pile 1.");
	ck_assert_msg(sol->visual->piles[6]->card_count == 7, "Incorrect number of cards in pile 2.");
	ck_assert_msg(sol->visual->piles[7]->card_count == 7, "Incorrect number of cards in pile 3.");
	ck_assert_msg(sol->visual->piles[8]->card_count == 7, "Incorrect number of cards in pile 4.");

	ck_assert_msg(sol->visual->piles[9]->card_count == 6, "Incorrect number of cards in pile 5.");
	ck_assert_msg(sol->visual->piles[10]->card_count == 6, "Incorrect number of cards in pile 6.");
	ck_assert_msg(sol->visual->piles[11]->card_count == 6, "Incorrect number of cards in pile 7.");
	ck_assert_msg(sol->visual->piles[12]->card_count == 6, "Incorrect number of cards in pile 8.");

	for(index=5;index<=8;++index) {
		for(index2=0;index2<7;++index2) {
			if(index2 <= 3) {
				ck_assert_msg(sol->visual->piles[index]->cards[index2]->card == 0, "Card shouldn't be revealed.");
			}
			else {
				ck_assert_msg(sol->visual->piles[index]->cards[index2]->card != 0, "Card should be revealed.");
			}
		}
	}

	for(index=9;index<=12;++index) {
		for(index2=0;index2<6;++index2) {
			ck_assert_msg(sol->visual->piles[index]->cards[index2]->card != 0, "Card should be revealed.");
		}
	}
}
END_TEST

START_TEST(test_sol_noname1_deal2) {
	solitaire *sol = solitaire_noname1();
	vis_pile *deck;
	int index;

	deck = sol->visual->piles[0];
	deck->pile_action(sol, deck);
	deck->pile_action(sol, deck);

	for(index=0;index<sol->visual->pile_count;++index) {
		if(index==0) {
			ck_assert_msg(sol->visual->piles[index]->card_count == 52, "All cards should have been moved back to the deck");
		}
		else {
			ck_assert_msg(sol->visual->piles[index]->card_count == 0, "There shouldn't be any cards here.");
		}
	}
	for(index=0;index<52;++index) {
		ck_assert_msg(sol->visual->piles[0]->cards[index]->card == 0, "Card should be hidden.");
	}
}
END_TEST

void add_sol_noname1_tests(Suite *suite) {
	TCase *sol_noname1 = tcase_create("Sol-Noname1");
	tcase_add_test(sol_noname1, test_sol_noname1_init);
	tcase_add_test(sol_noname1, test_sol_noname1_deal);
	tcase_add_test(sol_noname1, test_sol_noname1_deal2);
	suite_add_tcase(suite, sol_noname1);
}
