#include <stdlib.h>
#include "solitaire_maltesercross.h"

typedef struct {
	char state;

	card* deck[104];

	/* The four piles where we draw cards from. */
	card* src1[13];
	card* src2[13];
	card* src3[13];
	card* src4[13];

	/* The four diagonal piles where you build
	 * from king to ace. */
	card* king1[13];
	card* king2[13];
	card* king3[13];
	card* king4[13];

	/* Center pile build from ace to king. */
	card* center[13];

	/* Build from any card up. */
	card* build1[13];
	card* build2[13];
	card* build3[13];
	card* build4[13];

	/* All cards are moved to this pile. */
	card* done[104];

	card* pile1[13];
	card* pile2[13];
	card* pile3[13];
	card* pile4[13];
	card* pile5[13];
} internal;

static void sync_pile(card** src, int src_count, vis_pile* dest) {
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

	sync_pile(i->deck, 104, sol->visual->piles[0]);
	sync_pile(i->done, 104, sol->visual->piles[1]);
	sync_pile(i->center, 13, sol->visual->piles[2]);
	sync_pile(i->king1, 13, sol->visual->piles[3]);
	sync_pile(i->king2, 13, sol->visual->piles[4]);
	sync_pile(i->king3, 13, sol->visual->piles[5]);
	sync_pile(i->king4, 13, sol->visual->piles[6]);
	sync_pile(i->src1, 13, sol->visual->piles[7]);
	sync_pile(i->src2, 13, sol->visual->piles[8]);
	sync_pile(i->src3, 13, sol->visual->piles[9]);
	sync_pile(i->src4, 13, sol->visual->piles[10]);

	sync_pile(i->build1, 13, sol->visual->piles[11]);
	sync_pile(i->build2, 13, sol->visual->piles[12]);
	sync_pile(i->build3, 13, sol->visual->piles[13]);
	sync_pile(i->build4, 13, sol->visual->piles[14]);

	sync_pile(i->pile1, 13, sol->visual->piles[15]);
	sync_pile(i->pile2, 13, sol->visual->piles[16]);
	sync_pile(i->pile3, 13, sol->visual->piles[17]);
	sync_pile(i->pile4, 13, sol->visual->piles[18]);
	sync_pile(i->pile5, 13, sol->visual->piles[19]);
}

static void my_deal(solitaire* sol, vis_pile* pile) {
	internal* i = sol->data;

	if(i->state == 0) {
		card_move_count(i->src1, 13, i->deck, 104, 13);
		card_move_count(i->src2, 13, i->deck, 104, 13);
		card_move_count(i->src3, 13, i->deck, 104, 13);
		card_move_count(i->src4, 13, i->deck, 104, 13);
		i->state++;
	}

	if(i->state == 1) {
		/* Dealing 5 cards to the bottom 5 columns. This state is valid
		   until the deck is empty.
		*/

		if(card_count(i->deck, 104) == 0) {
			i->state = 2;
		}
	}

	sync(sol);
}

static void my_move(solitaire* sol, card_proxy* card_proxy) {
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
	visual_free(sol->visual);
	free(i);
	free(sol);
}

solitaire* solitaire_maltesercross() {
	vis_pile *deck, *done, *king1, *king2, *king3, *king4;
	vis_pile *src1, *src2, *src3, *src4;
	vis_pile *center, *build1, *build2, *build3, *build4;
	vis_pile *pile1, *pile2, *pile3, *pile4, *pile5;

	/* The one solitaire instance we have.*/
	solitaire* s = calloc(1, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = calloc(1, sizeof(internal));
	s->data = i;
	s->visual = visual_create();

	deck = pile_create(104);
	deck->origin[0] = 0 - (CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT * 2);
	deck->origin[1] = CARD_WIDTH;
	deck->rotation = -30.0f;
	deck->pile_action = my_deal;
	visual_add_pile(s->visual, deck);

	done = pile_create(104);
	done->origin[0] = CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT * 2;
	done->origin[1] = 0 - CARD_WIDTH;
	done->rotation = -30.0f;
	visual_add_pile(s->visual, done);

	center = pile_create(13);
	visual_add_pile(s->visual, center);

	king1 = pile_create(13);
	king1->origin[0] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king1->origin[1] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king1->rotation = -45.0f;
	visual_add_pile(s->visual, king1);

	king2 = pile_create(13);
	king2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king2->origin[1] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king2->rotation = 45.0f;
	visual_add_pile(s->visual, king2);

	king3 = pile_create(13);
	king3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king3->origin[1] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king3->rotation = 45.0f;
	visual_add_pile(s->visual, king3);

	king4 = pile_create(13);
	king4->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king4->origin[1] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king4->rotation = -45.0f;
	visual_add_pile(s->visual, king4);

	src1 = pile_create(13);
	src1->origin[1] = CARD_HEIGHT + CARD_SPACING;
	visual_add_pile(s->visual, src1);

	src2 = pile_create(13);
	src2->origin[1] = 0 - (CARD_HEIGHT + CARD_SPACING);
	visual_add_pile(s->visual, src2);

	src3 = pile_create(13);
	src3->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING + CARD_HEIGHT / 2);
	src3->rotation = 90.0f;
	visual_add_pile(s->visual, src3);

	src4 = pile_create(13);
	src4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING + CARD_HEIGHT / 2;
	src4->rotation = 90.0f;
	visual_add_pile(s->visual, src4);

	build1 = pile_create(13);
	build1->origin[0] = 0 - (CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT);
	visual_add_pile(s->visual, build1);

	build2 = pile_create(13);
	build2->origin[0] = CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT;
	visual_add_pile(s->visual, build2);

	build3 = pile_create(13);
	build3->origin[1] = CARD_HEIGHT / 2 + CARD_SPACING * 2 + CARD_HEIGHT + CARD_WIDTH / 2;
	build3->rotation = 90.0f;
	visual_add_pile(s->visual, build3);

	build4 = pile_create(13);
	build4->origin[1] = 0 - (CARD_HEIGHT / 2 + CARD_SPACING * 2 + CARD_HEIGHT + CARD_WIDTH / 2);
	build4->rotation = 90.0f;
	visual_add_pile(s->visual, build4);

	pile1 = pile_create(13);
	pile1->origin[0] = 0;
	pile1->origin[1] = 0;
	visual_add_pile(s->visual, pile1);

	pile2 = pile_create(13);
	pile2->origin[0] = 0;
	pile2->origin[1] = 0;
	visual_add_pile(s->visual, pile2);

	pile3 = pile_create(13);
	pile3->origin[0] = 0;
	pile3->origin[1] = 0;
	visual_add_pile(s->visual, pile3);

	pile4 = pile_create(13);
	pile4->origin[0] = 0;
	pile4->origin[1] = 0;
	visual_add_pile(s->visual, pile4);

	pile5 = pile_create(13);
	pile5->origin[0] = 0;
	pile5->origin[1] = 0;
	visual_add_pile(s->visual, pile5);

	create_deck(i->deck, 104);
	create_deck(i->deck, 104);

	sync(s);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->move = my_move;
	s->free = my_free;
	return s;
}
