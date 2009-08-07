#include <check.h>
#include "../lib/card.h"

START_TEST(test_create_deck) {
	card* list[52];
	int i;

	memset(&list, 0, sizeof(list));

	create_deck(list, 52);
	for(i=0;i<52;++i) {
		ck_assert_msg(list[i] != 0, "Card pointer was NULL at index: %d", i);
		ck_assert_msg(list[i]->value >= 1 && list[i]->value <= 13, "Card value isn't in range at index: %d", i);
		ck_assert_msg(list[i]->suit == e_diamonds ||
					  list[i]->suit == e_clubs ||
					  list[i]->suit == e_hearts ||
					  list[i]->suit == e_spades,
					  "Suit wasn't correct: %d", list[i]->suit);
		ck_assert_msg(list[i]->proxy != 0, "Card should point to a proxy.");
		ck_assert_msg(list[i]->proxy->card == 0, "Proxy shouldn't reveal the card.");
	}
}
END_TEST

START_TEST(test_card_count) {
	card c1, c2;
	card* cards[3];

	cards[0] = &c1;
	cards[1] = 0;
	cards[2] = &c2;

	ck_assert_msg(card_count(cards, 3) == 2, "There should be two cards in this pile.");
}
END_TEST

START_TEST(test_card_take_last) {
	card c1, c2;
	card *cards[3], *last;

	cards[0] = &c1;
	cards[1] = 0;
	cards[2] = &c2;

	last = card_take_last(cards, 3);
	ck_assert_msg(&c2 == last, "The last card wasn't taken.");
	ck_assert_msg(cards[2] == 0, "The card has been taken. This index should be 0.");
}
END_TEST

START_TEST(test_card_append) {
	card* cards[3] = {0, 0, 0};
	card c1;

	card_append(&c1, cards, 3);

	ck_assert_msg(cards[0] == &c1, "First card should be set.");
}
END_TEST

START_TEST(test_card_first_free) {
	card c1;
	card* cards[3];

	cards[0] = cards[1] = &c1;
	cards[2] = 0;

	ck_assert_msg(card_first_free(cards, 3) == 2, "Didn't return the correct index for the first free position.");

	cards[2] = &c1;
	ck_assert_msg(card_first_free(cards, 3) == -1, "Didn't return -1 for non free array.");
}
END_TEST

START_TEST(test_card_move_all) {
	card c1, c2;
	card* src[3];
	card* dst[3];
	src[0] = &c1;
	src[1] = &c2;
	src[2] = 0;
	dst[0] = dst[1] = dst[2] = 0;

	card_move_all(dst, 3, src, 3);

	ck_assert_msg(dst[0] == &c1, "c1 card wasn't appended.");
	ck_assert_msg(dst[1] == &c2, "c2 card wasn't appended.");
	ck_assert_msg(src[0] == 0, "c1 origin wasn't cleared.");
	ck_assert_msg(src[1] == 0, "c2 origin wasn't cleared.");
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
