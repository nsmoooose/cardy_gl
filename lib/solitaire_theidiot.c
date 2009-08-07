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

static void sync_pile(pile *src, vis_pile* dest) {
	int i;

	dest->card_count = 0;
	for(i=0;i<src->card_count;++i) {
		if(src->cards[i] == 0) {
			/* No card. */
			dest->first[i] = 0;
		}
		else {
			/* Yes there was a card. */
			dest->first[i] = src->cards[i]->proxy;
			dest->card_count++;
		}
	}
}

static void sync(solitaire* sol) {
	internal* i = sol->data;
	sync_pile(i->deck, sol->visual->piles[0]);
	sync_pile(i->pile1, sol->visual->piles[1]);
	sync_pile(i->pile2, sol->visual->piles[2]);
	sync_pile(i->pile3, sol->visual->piles[3]);
	sync_pile(i->pile4, sol->visual->piles[4]);
	sync_pile(i->done, sol->visual->piles[5]);
}

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

	sync(sol);
}

static void my_move(solitaire* sol, card_proxy* card_proxy) {
	sync(sol);
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

solitaire* solitaire_theidiot() {
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

	deck = vis_pile_create(52);
	deck->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	deck->origin[1] = 40.0f;
	deck->rotation = 45.0f;
	deck->pile_action = my_deal;
	visual_add_pile(s->visual, deck);

	pile1 = vis_pile_create(13);
	pile1->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	pile1->origin[1] = 70.0f;
	pile1->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile1);

	pile2 = vis_pile_create(13);
	pile2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	pile2->origin[1] = 70.0f;
	pile2->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile2);

	pile3 = vis_pile_create(13);
	pile3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	pile3->origin[1] = 70.0f;
	pile3->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile3);

	pile4 = vis_pile_create(13);
	pile4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	pile4->origin[1] = 70.0f;
	pile4->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile4);

	done = vis_pile_create(48);
	done->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2;
	done->origin[1] = 40.0f;
	done->rotation = -45.0f;
	visual_add_pile(s->visual, done);

	create_deck(i->deck);

	sync(s);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	s->move = my_move;
	s->free = my_free;
	return s;
}
