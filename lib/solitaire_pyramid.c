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

static void my_deal(solitaire* sol, vis_pile* pile) {
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

static void my_free(solitaire* sol) {
	internal* i = sol->data;

	pile_free(i->deck);
	pile_free(i->pile1);
	pile_free(i->pile2);
	pile_free(i->pile3);
	pile_free(i->pile4);
	pile_free(i->done);

	free(i);
	free(sol);
}

solitaire* solitaire_pyramid() {
	card *ace;
	vis_pile *deck, *pile1, *pile2, *pile3, *pile4, *done;

	/* The one solitaire instance we have.*/
	solitaire* s = calloc(1, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = calloc(1, sizeof(internal));
	s->data = i;

	s->visual = visual_create();

	i->deck = pile_create(52);
	i->pile1 = pile_create(13);
	i->pile2 = pile_create(13);
	i->pile3 = pile_create(13);
	i->pile4 = pile_create(13);
	i->done = pile_create(48);

	deck = vis_pile_create(i->deck);
	deck->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	deck->origin[1] = 40.0f;
	deck->rotation = 45.0f;
	deck->pile_action = my_deal;
	visual_add_pile(s->visual, deck);

	pile1 = vis_pile_create(i->pile1);
	pile1->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	pile1->origin[1] = 70.0f;
	pile1->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile1);

	pile2 = vis_pile_create(i->pile2);
	pile2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	pile2->origin[1] = 70.0f;
	pile2->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile2);

	pile3 = vis_pile_create(i->pile3);
	pile3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	pile3->origin[1] = 70.0f;
	pile3->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile3);

	pile4 = vis_pile_create(i->pile4);
	pile4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	pile4->origin[1] = 70.0f;
	pile4->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile4);

	done = vis_pile_create(i->done);
	done->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2;
	done->origin[1] = 40.0f;
	done->rotation = -45.0f;
	visual_add_pile(s->visual, done);

	create_deck(i->deck);

	ace = card_create(e_spades, 1);
	card_reveal(ace),
	card_append(ace, i->pile1);

	visual_sync(s->visual);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	s->free = my_free;
	return s;
}
