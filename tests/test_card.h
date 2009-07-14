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
