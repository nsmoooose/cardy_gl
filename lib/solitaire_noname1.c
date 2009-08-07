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
			dest->first[i] = src[i]->proxy;
			dest->card_count++;
		}
	}
}

static void sync(solitaire* sol) {
	internal* i = sol->data;

	sync_pile(i->deck, 52, sol->visual->piles[0]);
	sync_pile(i->ace1, 13, sol->visual->piles[1]);
	sync_pile(i->ace2, 13, sol->visual->piles[2]);
	sync_pile(i->ace3, 13, sol->visual->piles[3]);
	sync_pile(i->ace4, 13, sol->visual->piles[4]);
	sync_pile(i->pile1, 13, sol->visual->piles[5]);
	sync_pile(i->pile2, 13, sol->visual->piles[6]);
	sync_pile(i->pile3, 13, sol->visual->piles[7]);
	sync_pile(i->pile4, 13, sol->visual->piles[8]);
	sync_pile(i->pile5, 13, sol->visual->piles[9]);
	sync_pile(i->pile6, 13, sol->visual->piles[10]);
	sync_pile(i->pile7, 13, sol->visual->piles[11]);
	sync_pile(i->pile8, 13, sol->visual->piles[12]);
}
/*

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
*/

static void my_move(solitaire* sol, card_proxy* card_proxy) {
	/* internal* i = sol->data; */

	sync(sol);
}

static void my_free(solitaire* sol) {
	internal* i = sol->data;
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
	pile *deck, *ace1, *ace2, *ace3, *ace4;
	pile *pile1, *pile2, *pile3, *pile4, *pile5, *pile6, *pile7, *pile8;

	/* The one solitaire instance we have.*/
	solitaire* s = calloc(1, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = calloc(1, sizeof(internal));
	s->data = i;
	s->visual = visual_create();

	deck = pile_create(52);
	deck->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	deck->origin[1] = 70.0f;
	deck->rotation = 45.0f;
	visual_add_pile(s->visual, deck);

	ace1 = pile_create(13);
	ace1->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	ace1->origin[1] = 70.0f;
	visual_add_pile(s->visual, ace1);

	ace2 = pile_create(13);
	ace2->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	ace2->origin[1] = 70.0f;
	visual_add_pile(s->visual, ace2);

	ace3 = pile_create(13);
	ace3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2;
	ace3->origin[1] = 70.0f;
	visual_add_pile(s->visual, ace3);

	ace4 = pile_create(13);
	ace4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 3 + CARD_SPACING * 3;
	ace4->origin[1] = 70.0f;
	visual_add_pile(s->visual, ace4);

	pile1 = pile_create(52);
	pile1->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 3 + CARD_SPACING * 3);
	pile1->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile1);

	pile2 = pile_create(52);
	pile2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2);
	pile2->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile2);

	pile3 = pile_create(52);
	pile3->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	pile3->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile3);

	pile4 = pile_create(52);
	pile4->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	pile4->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile4);

	pile5 = pile_create(52);
	pile5->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	pile5->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile5);

	pile6 = pile_create(52);
	pile6->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	pile6->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile6);

	pile7 = pile_create(52);
	pile7->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2;
	pile7->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile7);

	pile8 = pile_create(52);
	pile8->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 3 + CARD_SPACING * 3;
	pile8->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile8);

	create_deck(i->deck, 52);

	sync(s);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->move = my_move;
	s->free = my_free;
	return s;
}