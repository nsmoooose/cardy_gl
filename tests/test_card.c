#include <check.h>
#include <stdlib.h>
#include "../api/card.h"

START_TEST(test_card_create_deck) {
	pile *deck;
	int i;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);

	card_create_deck(context, deck, 1);
	for (i = 0; i < 52; ++i) {
		ck_assert_msg(deck->cards[i] != 0, "Card pointer was NULL at index: %d",
		              i);
		ck_assert_msg(deck->cards[i]->value >= 1 && deck->cards[i]->value <= 13,
		              "Card value isn't in range at index: %d", i);
		ck_assert_msg(deck->cards[i]->suit == e_diamonds ||
		                  deck->cards[i]->suit == e_clubs ||
		                  deck->cards[i]->suit == e_hearts ||
		                  deck->cards[i]->suit == e_spades,
		              "Suit wasn't correct: %d", deck->cards[i]->suit);
		ck_assert_msg(deck->cards[i]->proxy != 0,
		              "Card should point to a proxy.");
		ck_assert_msg(deck->cards[i]->proxy->card == 0,
		              "Proxy shouldn't reveal the card.");
	}
}
END_TEST

START_TEST(test_card_create) {
	mem_context *context = mem_context_create();
	card *card = card_create(context, e_spades, 4);

	ck_assert_msg(context->blocks[0] == card, "No allocation made for card.");
	ck_assert_msg(context->blocks[1] == card->proxy, "Proxy not allocated.");
	ck_assert_msg(card->suit == e_spades, "Incorrect suit on card.");
	ck_assert_msg(card->value == 4, "Incorrect value on card.");
	ck_assert_msg(card->proxy->card == 0, "Card shouldn't be revealed.");
}
END_TEST

START_TEST(test_card_free) {
	mem_context *context = mem_context_create();
	card *card = card_create(context, e_spades, 4);

	card_free(context, card);

	ck_assert_msg(context->blocks[0] == 0, "Card wasn't freed.");
	ck_assert_msg(context->blocks[1] == 0, "Proxy wasn't freed.");
}
END_TEST

START_TEST(test_pile_create) {
	mem_context *context = mem_context_create();
	pile *p = pile_create(context, 3);

	ck_assert_msg(context->blocks[0] == p, "Pile allocation not done.");
	ck_assert_msg(context->blocks[1] == p->cards, "Array allocation not done.");
}
END_TEST

START_TEST(test_card_reveal) {
	mem_context *context = mem_context_create();
	card *card = card_create(context, e_spades, 4);

	card_reveal(card);

	ck_assert_msg(card->proxy->card != 0, "Card should have been revealed.");
}
END_TEST

START_TEST(test_card_last) {
	pile *deck;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 104);

	ck_assert_msg(card_last(deck) == 0,
	              "Last card should return 0 when no cards present.");

	card_create_deck(context, deck, 1);
	ck_assert_msg(card_last(deck) == deck->cards[51],
	              "Last card should be the last in the deck.");
}
END_TEST

START_TEST(test_card_reveal_count) {
	int index;
	mem_context *context = mem_context_create();
	pile *deck = pile_create(context, 52);

	card_create_deck(context, deck, 1);
	card_reveal_count(deck, 3, 5);

	for (index = 0; index < 52; ++index) {
		if (index >= 3 && index <= 7) {
			ck_assert_msg(deck->cards[index]->proxy->card != 0,
			              "Card should have been revealed.");
		} else {
			ck_assert_msg(deck->cards[index]->proxy->card == 0,
			              "Card should still be hidden.");
		}
	}
}
END_TEST

START_TEST(test_card_reveal_all) {
	int index;
	mem_context *context = mem_context_create();
	pile *deck = pile_create(context, 104);

	card_create_deck(context, deck, 1);
	card_reveal_all(deck);

	for (index = 0; index < 52; ++index) {
		ck_assert_msg(deck->cards[index]->proxy->card != 0,
		              "Card should have been revealed.");
	}
}
END_TEST

START_TEST(test_card_reveal_all_array) {
	mem_context *context = mem_context_create();
	pile *p1 = pile_create(context, 104);
	pile *p2 = pile_create(context, 104);
	p1->cards[0] = card_create(context, e_spades, 1);
	p2->cards[0] = card_create(context, e_clubs, 1);

	card_reveal_all_array(2, p1, p2);

	ck_assert_msg(p1->cards[0]->proxy->card != 0,
	              "Card should have been revealed.");
	ck_assert_msg(p2->cards[0]->proxy->card != 0,
	              "Card should have been revealed.");
}
END_TEST

START_TEST(test_card_hide) {
	mem_context *context = mem_context_create();
	card *card = card_create(context, e_spades, 5);

	card_reveal(card);
	card_hide(card);

	ck_assert_msg(card->proxy->card == 0, "Card should have been hidden.");
}
END_TEST

START_TEST(test_card_hide_count) {
	int index;
	mem_context *context = mem_context_create();
	pile *deck = pile_create(context, 104);

	card_create_deck(context, deck, 1);
	card_reveal_all(deck);
	card_hide_count(deck, 3, 5);

	for (index = 0; index < 52; ++index) {
		if (index >= 3 && index <= 7) {
			ck_assert_msg(deck->cards[index]->proxy->card == 0,
			              "Card should be hidden.");
		} else {
			ck_assert_msg(deck->cards[index]->proxy->card != 0,
			              "Card should be revealed.");
		}
	}
}
END_TEST

START_TEST(test_card_hide_all) {
	int index;
	mem_context *context = mem_context_create();
	pile *deck = pile_create(context, 104);

	card_create_deck(context, deck, 1);
	card_reveal_all(deck);
	card_hide_all(deck);

	for (index = 0; index < 52; ++index) {
		ck_assert_msg(deck->cards[index]->proxy->card == 0,
		              "Card should have been hidden.");
	}
}
END_TEST

START_TEST(test_card_count) {
	card c1, c2;
	mem_context *context = mem_context_create();
	pile *pile = pile_create(context, 3);

	pile->cards[0] = &c1;
	pile->cards[2] = &c2;

	ck_assert_msg(card_count(pile) == 2,
	              "There should be two cards in this pile.");
}
END_TEST

START_TEST(test_card_take_last) {
	card c1, c2, *last;
	pile *pile;
	mem_context *context = mem_context_create();
	pile = pile_create(context, 3);

	ck_assert_msg(card_take_last(pile) == 0,
	              "When no cards present should return 0.");

	pile->cards[0] = &c1;
	pile->cards[2] = &c2;

	last = card_take_last(pile);
	ck_assert_msg(&c2 == last, "The last card wasn't taken.");
	ck_assert_msg(pile->cards[2] == 0,
	              "The card has been taken. This index should be 0.");
}
END_TEST

START_TEST(test_card_append) {
	pile *pile;
	card c1;
	mem_context *context = mem_context_create();

	pile = pile_create(context, 3);
	card_append(&c1, pile);

	ck_assert_msg(pile->cards[0] == &c1, "First card should be set.");
}
END_TEST

START_TEST(test_card_first_free) {
	card c1;
	mem_context *context = mem_context_create();
	pile *pile = pile_create(context, 3);

	pile->cards[0] = pile->cards[1] = &c1;

	ck_assert_msg(
		card_first_free(pile) == 2,
		"Didn't return the correct index for the first free position.");

	pile->cards[2] = &c1;
	ck_assert_msg(card_first_free(pile) == -1,
	              "Didn't return -1 for non free array.");
}
END_TEST

START_TEST(test_card_move_all) {
	card c1, c2;
	pile *src;
	pile *dst;
	mem_context *context = mem_context_create();

	src = pile_create(context, 3);
	dst = pile_create(context, 3);
	src->cards[0] = &c1;
	src->cards[1] = &c2;

	card_move_all(dst, src);

	ck_assert_msg(dst->cards[0] == &c1, "c1 card wasn't appended.");
	ck_assert_msg(dst->cards[1] == &c2, "c2 card wasn't appended.");
	ck_assert_msg(src->cards[0] == 0, "c1 origin wasn't cleared.");
	ck_assert_msg(src->cards[1] == 0, "c2 origin wasn't cleared.");
}
END_TEST

START_TEST(test_card_move_all_array) {
	pile *src1, *src2;
	pile *dst;
	mem_context *context = mem_context_create();

	src1 = pile_create(context, 52);
	src2 = pile_create(context, 52);
	dst = pile_create(context, 104);
	card_create_deck(context, src1, 1);
	card_create_deck(context, src2, 1);

	card_move_all_array(dst, 2, src1, src2);

	ck_assert_msg(card_count(src1) == 0, "Source 1 card count should be 0");
	ck_assert_msg(card_count(src2) == 0, "Source 2 card count should be 0");
	ck_assert_msg(card_count(dst) == 104,
	              "Destination card count should be 104");
}
END_TEST

START_TEST(test_visual_pile_create) {
	pile *src;
	visual_pile *pile;
	mem_context *context = mem_context_create();

	src = pile_create(context, 2);
	pile = visual_pile_create(context, src);

	ck_assert_msg(context->blocks[2] == pile, "Pile allocation not done.");
	ck_assert_msg(context->blocks[3] == pile->cards,
	              "Card array not allocated.");
	ck_assert_msg(pile->card_count = 2,
	              "Card count should be same as pile count.");
	ck_assert_msg(pile->data == src, "data member should be set.");
}
END_TEST

START_TEST(test_visual_create) {
	mem_context *context = mem_context_create();
	visual_settings *settings = calloc(1, sizeof(visual_settings));
	visual *vis = visual_create(context, settings);

	ck_assert_msg(context->blocks[0] == vis, "No allocation made.");
	ck_assert_msg(vis->piles == 0, "piles member should be initialized to 0.");
	ck_assert_msg(vis->pile_count == 0, "No of piles shall be 0");
	ck_assert_msg(vis->settings == settings, "Settings wasn't assigned.");
}
END_TEST

START_TEST(test_visual_add_pile) {
	mem_context *context = mem_context_create();
	pile *src1 = pile_create(context, 10);
	pile *src2 = pile_create(context, 10);
	visual *vis = visual_create(context, 0);
	visual_pile *pile1 = visual_pile_create(context, src1);
	visual_pile *pile2 = visual_pile_create(context, src2);

	visual_add_pile(context, vis, pile1);

	ck_assert_msg(vis->pile_count == 1,
	              "pile_count should be 1 since one pile has been added.");
	ck_assert_msg(vis->piles[0] == pile1, "The first pile should be assigned.");

	visual_add_pile(context, vis, pile2);
	ck_assert_msg(vis->pile_count == 2,
	              "pile_count should be 2 since two pile has been added.");
	ck_assert_msg(vis->piles[0] == pile1, "The first pile should be assigned.");
	ck_assert_msg(vis->piles[1] == pile2,
	              "The second pile should be assigned.");
}
END_TEST

START_TEST(test_visual_sync) {
	mem_context *context = mem_context_create();
	pile *pile = pile_create(context, 10);
	visual *vis = visual_create(context, 0);
	visual_pile *visual_pile = visual_pile_create(context, pile);
	card *card = card_create(context, e_spades, 3);
	card_reveal(card);

	card_append(card, pile);
	visual_add_pile(context, vis, visual_pile);
	visual_sync(vis);

	ck_assert_msg(vis->piles[0]->cards[0]->card == card,
	              "first card wasn't synced.");
}
END_TEST

START_TEST(test_card_take_match) {
	card *taken_card, prototype;

	mem_context *context = mem_context_create();
	pile *pile = pile_create(context, 52);
	card_create_deck(context, pile, 1);

	prototype.value = 13;
	taken_card = card_take_match(pile, card_match_value, &prototype);

	ck_assert(taken_card != 0);
	ck_assert(taken_card->value == 13);
	ck_assert(card_count(pile) == 51);
}
END_TEST

void add_card_tests(Suite *suite) {
	TCase *c = tcase_create("Card");
	tcase_add_test(c, test_card_create);
	tcase_add_test(c, test_card_free);
	tcase_add_test(c, test_pile_create);
	tcase_add_test(c, test_card_create_deck);
	tcase_add_test(c, test_card_count);
	tcase_add_test(c, test_card_take_last);
	tcase_add_test(c, test_card_append);
	tcase_add_test(c, test_card_last);
	tcase_add_test(c, test_card_move_all);
	tcase_add_test(c, test_card_move_all_array);
	tcase_add_test(c, test_card_first_free);
	tcase_add_test(c, test_visual_pile_create);
	tcase_add_test(c, test_visual_create);
	tcase_add_test(c, test_visual_add_pile);
	tcase_add_test(c, test_visual_sync);
	tcase_add_test(c, test_card_reveal);
	tcase_add_test(c, test_card_reveal_count);
	tcase_add_test(c, test_card_reveal_all);
	tcase_add_test(c, test_card_reveal_all_array);
	tcase_add_test(c, test_card_hide);
	tcase_add_test(c, test_card_hide_count);
	tcase_add_test(c, test_card_hide_all);
	tcase_add_test(c, test_card_take_match);
	suite_add_tcase(suite, c);
}
