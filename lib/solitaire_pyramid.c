#include <stdlib.h>
#include "solitaire_theidiot.h"

typedef struct {
	pile* deck;

	pile* pile1;
	pile* pile2;
	pile* pile3;
	pile* pile4;

	pile* done;
} internal;

static void my_new_game(solitaire* sol) {
}

static void my_deal(solitaire* sol, visual_pile* pile) {
	internal* i = sol->data;

	if(card_count(i->deck) >= 4) {
		card* card1 = card_take_last(i->deck);
		card* card2 = card_take_last(i->deck);
		card* card3 = card_take_last(i->deck);
		card* card4 = card_take_last(i->deck);

		card_reveal(card1);
		card_reveal(card2);
		card_reveal(card3);
		card_reveal(card4);

		card_append(card1, i->pile1);
		card_append(card2, i->pile2);
		card_append(card3, i->pile3);
		card_append(card4, i->pile4);
	}

	visual_sync(sol->visual);
}

solitaire* solitaire_pyramid(mem_context *context) {
	card *ace;
	visual_pile *deck, *pile1, *pile2, *pile3, *pile4, *done;

	/* The one solitaire instance we have.*/
	solitaire* s = mem_alloc(context, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = mem_alloc(context, sizeof(internal));
	s->data = i;

	s->visual = visual_create(context);

	i->deck = pile_create(context, 52);
	i->pile1 = pile_create(context, 13);
	i->pile2 = pile_create(context, 13);
	i->pile3 = pile_create(context, 13);
	i->pile4 = pile_create(context, 13);
	i->done = pile_create(context, 48);

	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	deck->origin[1] = 40.0f;
	deck->rotation = 45.0f;
	deck->pile_action = my_deal;
	visual_add_pile(context, s->visual, deck);

	pile1 = visual_pile_create(context, i->pile1);
	pile1->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	pile1->origin[1] = 70.0f;
	pile1->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(context, s->visual, pile1);

	pile2 = visual_pile_create(context, i->pile2);
	pile2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	pile2->origin[1] = 70.0f;
	pile2->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(context, s->visual, pile2);

	pile3 = visual_pile_create(context, i->pile3);
	pile3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	pile3->origin[1] = 70.0f;
	pile3->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(context, s->visual, pile3);

	pile4 = visual_pile_create(context, i->pile4);
	pile4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	pile4->origin[1] = 70.0f;
	pile4->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(context, s->visual, pile4);

	done = visual_pile_create(context, i->done);
	done->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2;
	done->origin[1] = 40.0f;
	done->rotation = -45.0f;
	visual_add_pile(context, s->visual, done);

	create_deck(context, i->deck);

	ace = card_create(context, e_spades, 1);
	card_reveal(ace),
	card_append(ace, i->pile1);

	visual_sync(s->visual);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	return s;
}
