#include <stdlib.h>
#include "solitaire_maltesercross.h"

typedef struct {
	card* deck[52];

	/* The four piles where we draw cards from. */
	card* ace1[13];
	card* ace2[13];
	card* ace3[13];
	card* ace4[13];

	/* The four diagonal piles where you build
	 * from king to ace. */
	card* pile1[52];
	card* pile2[52];
	card* pile3[52];
	card* pile4[52];
	card* pile5[52];
	card* pile6[52];
	card* pile7[52];
	card* pile8[52];

	pile vis_deck;
	pile vis_ace1;
	pile vis_ace2;
	pile vis_ace3;
	pile vis_ace4;
	pile vis_1;
	pile vis_2;
	pile vis_3;
	pile vis_4;
	pile vis_5;
	pile vis_6;
	pile vis_7;
	pile vis_8;

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
	sync_pile(i->deck, 52, &i->vis_deck);
	sync_pile(i->ace1, 13, &i->vis_ace1);
	sync_pile(i->ace2, 13, &i->vis_ace2);
	sync_pile(i->ace3, 13, &i->vis_ace3);
	sync_pile(i->ace4, 13, &i->vis_ace4);
	sync_pile(i->pile1, 13, &i->vis_1);
	sync_pile(i->pile2, 13, &i->vis_2);
	sync_pile(i->pile3, 13, &i->vis_3);
	sync_pile(i->pile4, 13, &i->vis_4);
	sync_pile(i->pile5, 13, &i->vis_5);
	sync_pile(i->pile6, 13, &i->vis_6);
	sync_pile(i->pile7, 13, &i->vis_7);
	sync_pile(i->pile8, 13, &i->vis_8);
}

static void my_deal(solitaire* sol, pile* pile) {
/*
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
*/
}

static int my_get_pile_count(struct solitaire_St* sol) {
	/* We have 6 piles of cards. The deck. the 4 piles
	 * with cards and the pile with thrown away cards.*/
	return 13;
}

static pile* my_get_pile(struct solitaire_St* sol, int no) {
	internal* i = sol->data;

	switch(no) {
	case 0:
		return &i->vis_deck;
	case 1:
		return &i->vis_ace1;
	case 2:
		return &i->vis_ace2;
	case 3:
		return &i->vis_ace3;
	case 4:
		return &i->vis_ace4;
	case 5:
		return &i->vis_1;
	case 6:
		return &i->vis_2;
	case 7:
		return &i->vis_3;
	case 8:
		return &i->vis_4;
	case 9:
		return &i->vis_5;
	case 10:
		return &i->vis_6;
	case 11:
		return &i->vis_7;
	case 12:
		return &i->vis_8;
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
	card_append_all(i->deck, 52, i->done, 48);
	card_append_all(i->deck, 52, i->pile1, 13);
	card_append_all(i->deck, 52, i->pile2, 13);
	card_append_all(i->deck, 52, i->pile3, 13);
	card_append_all(i->deck, 52, i->pile4, 13);

	for(index=0;index<52;++index) {
		if(i->deck[index]) {
			free(i->deck[index]);
		}
	}
*/
	free(i);
	free(sol);
}

solitaire* solitaire_noname1() {
	/* The one solitaire instance we have.*/
	solitaire* s = calloc(1, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = calloc(1, sizeof(internal));
	s->data = i;

	i->vis_deck.first = calloc(52, sizeof(card_proxy*));
	i->vis_ace1.first = calloc(13, sizeof(card_proxy*));
	i->vis_ace2.first = calloc(13, sizeof(card_proxy*));
	i->vis_ace3.first = calloc(13, sizeof(card_proxy*));
	i->vis_ace4.first = calloc(13, sizeof(card_proxy*));
	i->vis_1.first = calloc(52, sizeof(card_proxy*));
	i->vis_2.first = calloc(52, sizeof(card_proxy*));
	i->vis_3.first = calloc(52, sizeof(card_proxy*));
	i->vis_4.first = calloc(52, sizeof(card_proxy*));
	i->vis_5.first = calloc(52, sizeof(card_proxy*));
	i->vis_6.first = calloc(52, sizeof(card_proxy*));
	i->vis_7.first = calloc(52, sizeof(card_proxy*));
	i->vis_8.first = calloc(52, sizeof(card_proxy*));

	i->vis_deck.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	i->vis_deck.origin[1] = 70.0f;
	i->vis_deck.rotation = 45.0f;

	i->vis_ace1.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	i->vis_ace2.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	i->vis_ace3.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2;
	i->vis_ace4.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 3 + CARD_SPACING * 3;
	i->vis_ace1.origin[1] = 70.0f;
	i->vis_ace2.origin[1] = 70.0f;
	i->vis_ace3.origin[1] = 70.0f;
	i->vis_ace4.origin[1] = 70.0f;

	i->vis_1.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 3 + CARD_SPACING * 3);
	i->vis_2.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2);
	i->vis_3.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	i->vis_4.origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	i->vis_5.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	i->vis_6.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	i->vis_7.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2;
	i->vis_8.origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 3 + CARD_SPACING * 3;

	i->vis_1.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_2.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_3.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_4.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_5.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_6.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_7.translateY = 0 - CARD_HEIGHT / 5;
	i->vis_8.translateY = 0 - CARD_HEIGHT / 5;

	create_deck(i->deck, 52);

	sync(i);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->get_pile_count = my_get_pile_count;
	s->get_pile = my_get_pile;
	s->move = my_move;
	s->free = my_free;
	return s;
}
