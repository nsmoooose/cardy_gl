#include <stdlib.h>
#include "solitaire_theidiot.h"

typedef struct {
	card* deck[52];

	card* pile1[13];
	card* pile2[13];
	card* pile3[13];
	card* pile4[13];

	card* done[52];

	pile deck_pile;
	pile pile1_pile;
	pile pile2_pile;
	pile pile3_pile;
	pile pile4_pile;
	pile done_pile;
} internal;

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
}

static void my_free(solitaire* sol) {
	internal* i = sol->data;
	int index;

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

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->deal = my_deal;
	s->get_pile_count = my_get_pile_count;
	s->get_pile = my_get_pile;
	s->move = my_move;
	s->free = my_free;
	create_deck(i->deck, 52);
	return s;
}
