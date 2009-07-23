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
	int i;

	dest->card_count = 0;
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

static void my_deal(solitaire* sol, pile* pile) {
	internal* i = sol->data;

	if(card_count(i->deck, 52) >= 4) {
		card* card1 = card_take_last(i->deck, 52);
		card* card2 = card_take_last(i->deck, 52);
		card* card3 = card_take_last(i->deck, 52);
		card* card4 = card_take_last(i->deck, 52);

		card_reveal(card1);
		card_reveal(card2);
		card_reveal(card3);
		card_reveal(card4);

		card_append(card1, i->pile1, 13);
		card_append(card2, i->pile2, 13);
		card_append(card3, i->pile3, 13);
		card_append(card4, i->pile4, 13);
	}

	sync(i);
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

	card_append_all(i->deck, 52, i->done, 52);
	card_append_all(i->deck, 52, i->pile1, 13);
	card_append_all(i->deck, 52, i->pile2, 13);
	card_append_all(i->deck, 52, i->pile3, 13);
	card_append_all(i->deck, 52, i->pile4, 13);


	/*
	 * Free the proxy data.
	 * Free piles and vectors of proxy cards.
	 */

	for(index=0;index<52;++index) {
		if(i->deck[index]) {
			free(i->deck[index]);
		}
	}
	/* free(i); */
	/* free(sol); */
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

	i->deck_pile.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	i->pile1_pile.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	i->pile2_pile.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	i->pile3_pile.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	i->pile4_pile.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	i->done_pile.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2;

	i->deck_pile.origin[1] = 40.0f;
	i->deck_pile.rotation = 45.0f;
	i->deck_pile.pile_action = my_deal;

	i->pile1_pile.origin[1] = 70.0f;
	i->pile2_pile.origin[1] = 70.0f;
	i->pile3_pile.origin[1] = 70.0f;
	i->pile4_pile.origin[1] = 70.0f;

	i->pile1_pile.translateY = 0 - CARD_HEIGHT / 5;
	i->pile2_pile.translateY = 0 - CARD_HEIGHT / 5;
	i->pile3_pile.translateY = 0 - CARD_HEIGHT / 5;
	i->pile4_pile.translateY = 0 - CARD_HEIGHT / 5;

	i->done_pile.origin[1] = 40.0f;
	i->done_pile.rotation = -45.0f;

	create_deck(i->deck, 52);

	sync(i);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	s->get_pile_count = my_get_pile_count;
	s->get_pile = my_get_pile;
	s->move = my_move;
	s->free = my_free;
	return s;
}
