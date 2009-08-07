#include <check.h>
#include "../lib/card.h"

START_TEST(test_create_deck) {
	pile* deck;
	int i;

	deck = pile_create(52);

	create_deck(deck);
	for(i=0;i<52;++i) {
		ck_assert_msg(deck->cards[i] != 0, "Card pointer was NULL at index: %d", i);
		ck_assert_msg(deck->cards[i]->value >= 1 && deck->cards[i]->value <= 13, "Card value isn't in range at index: %d", i);
		ck_assert_msg(deck->cards[i]->suit == e_diamonds ||
					  deck->cards[i]->suit == e_clubs ||
					  deck->cards[i]->suit == e_hearts ||
					  deck->cards[i]->suit == e_spades,
					  "Suit wasn't correct: %d", deck->cards[i]->suit);
		ck_assert_msg(deck->cards[i]->proxy != 0, "Card should point to a proxy.");
		ck_assert_msg(deck->cards[i]->proxy->card == 0, "Proxy shouldn't reveal the card.");
	}
}
END_TEST

START_TEST(test_card_count) {
	card c1, c2;
	pile *pile = pile_create(3);

	pile->cards[0] = &c1;
	pile->cards[2] = &c2;

	ck_assert_msg(card_count(pile) == 2, "There should be two cards in this pile.");
}
END_TEST

START_TEST(test_card_take_last) {
	card c1, c2, *last;
	pile *pile;
	pile = pile_create(3);

	pile->cards[0] = &c1;
	pile->cards[2] = &c2;

	last = card_take_last(pile);
	ck_assert_msg(&c2 == last, "The last card wasn't taken.");
	ck_assert_msg(pile->cards[2] == 0, "The card has been taken. This index should be 0.");
}
END_TEST

START_TEST(test_card_append) {
	pile *pile;
	card c1;

	pile = pile_create(3);
	card_append(&c1, pile);

	ck_assert_msg(pile->cards[0] == &c1, "First card should be set.");
}
END_TEST

START_TEST(test_card_first_free) {
	card c1;
	pile* pile = pile_create(3);

	pile->cards[0] = pile->cards[1] = &c1;

	ck_assert_msg(card_first_free(pile) == 2, "Didn't return the correct index for the first free position.");

	pile->cards[2] = &c1;
	ck_assert_msg(card_first_free(pile) == -1, "Didn't return -1 for non free array.");
}
END_TEST

START_TEST(test_card_move_all) {
	card c1, c2;
	pile *src;
	pile *dst;

	src = pile_create(3);
	dst = pile_create(3);
	src->cards[0] = &c1;
	src->cards[1] = &c2;

	card_move_all(dst, src);

	ck_assert_msg(dst->cards[0] == &c1, "c1 card wasn't appended.");
	ck_assert_msg(dst->cards[1] == &c2, "c2 card wasn't appended.");
	ck_assert_msg(src->cards[0] == 0, "c1 origin wasn't cleared.");
	ck_assert_msg(src->cards[1] == 0, "c2 origin wasn't cleared.");
}
END_TEST

START_TEST(test_vis_pile_create) {
	vis_pile* pile;

	pile = vis_pile_create(2);
	pile->first[0] = 0;
	pile->first[1] = 0;
}
END_TEST

START_TEST(test_visual_create) {
	visual* vis = visual_create();

	ck_assert_msg(vis->piles == 0, "piles member should be initialized to 0.");
	ck_assert_msg(vis->pile_count == 0, "No of piles shall be 0");
}
END_TEST

START_TEST(test_visual_add_pile) {
	visual* vis = visual_create();
	vis_pile* pile1 = vis_pile_create(10);
	vis_pile* pile2 = vis_pile_create(10);

	visual_add_pile(vis, pile1);

	ck_assert_msg(vis->pile_count == 1, "pile_count should be 1 since one pile has been added.");
	ck_assert_msg(vis->piles[0] == pile1, "The first pile should be assigned.");

	visual_add_pile(vis, pile2);
	ck_assert_msg(vis->pile_count == 2, "pile_count should be 2 since two pile has been added.");
	ck_assert_msg(vis->piles[0] == pile1, "The first pile should be assigned.");
	ck_assert_msg(vis->piles[1] == pile2, "The second pile should be assigned.");
}
END_TEST
