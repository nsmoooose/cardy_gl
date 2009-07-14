#include <stdlib.h>
#include "solitaire_theidiot.h"

typedef struct {
	card* deck[52];

	card* pile1[13];
	card* pile2[13];
	card* pile3[13];
	card* pile4[13];

	card* done[48];

	pile deck_pile;
	pile pile1_pile;
	pile pile2_pile;
	pile pile3_pile;
	pile pile4_pile;
	pile done_pile;
} internal;

static void sync_pile(card** src, int src_count, pile* dest) {
	dest->card_count = 0;
	int i;
	for(i=0;i<src_count;++i) {
		if(src[i] == 0) {
			/* No card. */
			dest->first[i] = 0;
		}
		else {
			/* Yes there was a card. */
			dest->first[i] = (card_proxy*)src[i]->data;
			dest->card_count++;
		}
	}
}

static void sync(internal* i) {
	sync_pile(i->deck, 52, &i->deck_pile);
	sync_pile(i->pile1, 13, &i->pile1_pile);
	sync_pile(i->pile2, 13, &i->pile2_pile);
	sync_pile(i->pile3, 13, &i->pile3_pile);
	sync_pile(i->pile4, 13, &i->pile4_pile);
	sync_pile(i->done, 48, &i->done_pile);
}

static void my_new_game(solitaire* sol) {
}

static void my_deal(solitaire* sol) {
	/* Collect all cards back to the deck. */
	/* Randomize them. */
}

static int my_get_pile_count(struct solitaire_St* sol) {
	/* We have 6 piles of cards. The deck. the 4 piles
	 * with cards and the pile with thrown away cards.*/
	return 6;
}

static pile* my_get_pile(struct solitaire_St* sol, int no) {
	internal* i = sol->data;

	switch(no) {
	case 0:
		return &i->deck_pile;
	case 1:
		return &i->pile1_pile;
	case 2:
		return &i->pile2_pile;
	case 3:
		return &i->pile3_pile;
	case 4:
		return &i->pile4_pile;
	case 5:
		return &i->done_pile;
	default:
		return 0;
	}
}

static void my_move(solitaire* sol, card_proxy* card_proxy) {
	internal* i = sol->data;

	sync(i);
}

static void my_free(solitaire* sol) {
	internal* i = sol->data;
	int index;

	/*
	 * Free the proxy data.
	 * Free piles and vectors of proxy cards.
	 */

	for(index=0;index<52;++index) {
		if(i->deck[index]) {
			free(i->deck[index]);
		}
		if(i->done[index]) {
			free(i->done[index]);
		}
	}
	for(index=0;index<13;++index) {
		if(i->pile1[index]) {
			free(i->pile1[index]);
		}
		if(i->pile2[index]) {
			free(i->pile2[index]);
		}
		if(i->pile3[index]) {
			free(i->pile3[index]);
		}
		if(i->pile4[index]) {
			free(i->pile4[index]);
		}
	}
	free(i);
	free(sol);
}

solitaire* solitaire_theidiot() {
	/* The one solitaire instance we have.*/
	solitaire* s = calloc(1, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = calloc(1, sizeof(internal));
	s->data = i;

	i->deck_pile.first = calloc(52, sizeof(card_proxy*));
	i->pile1_pile.first = calloc(13, sizeof(card_proxy*));
	i->pile2_pile.first = calloc(13, sizeof(card_proxy*));
	i->pile3_pile.first = calloc(13, sizeof(card_proxy*));
	i->pile4_pile.first = calloc(13, sizeof(card_proxy*));
	i->done_pile.first = calloc(48, sizeof(card_proxy*));

	create_deck(i->deck, 52);

	sync(i);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	s->deal = my_deal;
	s->get_pile_count = my_get_pile_count;
	s->get_pile = my_get_pile;
	s->move = my_move;
	s->free = my_free;
	return s;
}
