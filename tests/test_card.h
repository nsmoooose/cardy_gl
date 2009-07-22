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
		ck_assert_msg(list[i]->data != 0, "Card should point to a proxy.");
		ck_assert_msg(((card_proxy*)list[i]->data)->card == 0, "Proxy shouldn't reveal the card.");
	}
}
END_TEST

START_TEST(test_card_count) {
	card c1, c2;

	card* cards[3] = {&c1, 0, &c2};

	ck_assert_msg(card_count(cards, 3) == 2, "There should be two cards in this pile.");
}
END_TEST

START_TEST(test_card_take_last) {
	card c1, c2;

	card* cards[3] = {&c1, 0, &c2};

	card* last = card_take_last(cards, 3);
	ck_assert_msg(&c2 == last, "The last card wasn't taken.");
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
	card* cards[3] = {1, 1, 0 };

	ck_assert_msg(card_first_free(cards, 3) == 2, "Didn't return the correct index for the first free position.");

	cards[2] = 1;
	ck_assert_msg(card_first_free(cards, 3) == -1, "Didn't return -1 for non free array.");
}
END_TEST
