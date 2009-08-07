#include <stdlib.h>
#include "solitaire_maltesercross.h"

typedef struct {
	char state;

	pile* deck;

	/* The four piles where we draw cards from. */
	pile* src1;
	pile* src2;
	pile* src3;
	pile* src4;

	/* The four diagonal piles where you build
	 * from king to ace. */
	pile* king1;
	pile* king2;
	pile* king3;
	pile* king4;

	/* Center pile build from ace to king. */
	pile* center;

	/* Build from any card up. */
	pile* build1;
	pile* build2;
	pile* build3;
	pile* build4;

	/* All cards are moved to this pile. */
	pile* done;

	pile* pile1;
	pile* pile2;
	pile* pile3;
	pile* pile4;
	pile* pile5;
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
	sync_pile(i->done, sol->visual->piles[1]);
	sync_pile(i->center, sol->visual->piles[2]);
	sync_pile(i->king1, sol->visual->piles[3]);
	sync_pile(i->king2, sol->visual->piles[4]);
	sync_pile(i->king3, sol->visual->piles[5]);
	sync_pile(i->king4, sol->visual->piles[6]);
	sync_pile(i->src1, sol->visual->piles[7]);
	sync_pile(i->src2, sol->visual->piles[8]);
	sync_pile(i->src3, sol->visual->piles[9]);
	sync_pile(i->src4, sol->visual->piles[10]);

	sync_pile(i->build1, sol->visual->piles[11]);
	sync_pile(i->build2, sol->visual->piles[12]);
	sync_pile(i->build3, sol->visual->piles[13]);
	sync_pile(i->build4, sol->visual->piles[14]);

	sync_pile(i->pile1, sol->visual->piles[15]);
	sync_pile(i->pile2, sol->visual->piles[16]);
	sync_pile(i->pile3, sol->visual->piles[17]);
	sync_pile(i->pile4, sol->visual->piles[18]);
	sync_pile(i->pile5, sol->visual->piles[19]);
}

static void my_deal(solitaire* sol, vis_pile* pile) {
	internal* i = sol->data;

	if(i->state == 0) {
		card_move_count(i->src1, i->deck, 13);
		card_move_count(i->src2, i->deck, 13);
		card_move_count(i->src3, i->deck, 13);
		card_move_count(i->src4, i->deck, 13);
		i->state++;
	}

	if(i->state == 1) {
		/* Dealing 5 cards to the bottom 5 columns. This state is valid
		   until the deck is empty.
		*/

		if(card_count(i->deck) == 0) {
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

	i->deck = pile_create(104);
	i->src1 = pile_create(13);
	i->src2 = pile_create(13);
	i->src3 = pile_create(13);
	i->src4 = pile_create(13);
	i->king1 = pile_create(13);
	i->king2 = pile_create(13);
	i->king3 = pile_create(13);
	i->king4 = pile_create(13);
	i->center = pile_create(13);
	i->build1 = pile_create(13);
	i->build2 = pile_create(13);
	i->build3 = pile_create(13);
	i->build4 = pile_create(13);
	i->done = pile_create(104);
	i->pile1 = pile_create(52);
	i->pile2 = pile_create(52);
	i->pile3 = pile_create(52);
	i->pile4 = pile_create(52);
	i->pile5 = pile_create(52);

	deck = vis_pile_create(i->deck);
	deck->origin[0] = 0 - (CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT * 2);
	deck->origin[1] = CARD_WIDTH;
	deck->rotation = -30.0f;
	deck->pile_action = my_deal;
	visual_add_pile(s->visual, deck);

	done = vis_pile_create(i->done);
	done->origin[0] = CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT * 2;
	done->origin[1] = 0 - CARD_WIDTH;
	done->rotation = -30.0f;
	visual_add_pile(s->visual, done);

	center = vis_pile_create(i->center);
	visual_add_pile(s->visual, center);

	king1 = vis_pile_create(i->king1);
	king1->origin[0] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king1->origin[1] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king1->rotation = -45.0f;
	visual_add_pile(s->visual, king1);

	king2 = vis_pile_create(i->king2);
	king2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king2->origin[1] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king2->rotation = 45.0f;
	visual_add_pile(s->visual, king2);

	king3 = vis_pile_create(i->king3);
	king3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2;
	king3->origin[1] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king3->rotation = 45.0f;
	visual_add_pile(s->visual, king3);

	king4 = vis_pile_create(i->king4);
	king4->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king4->origin[1] = 0 - (CARD_WIDTH / 2 + CARD_SPACING * 2 + CARD_HEIGHT / 2);
	king4->rotation = -45.0f;
	visual_add_pile(s->visual, king4);

	src1 = vis_pile_create(i->src1);
	src1->origin[1] = CARD_HEIGHT + CARD_SPACING;
	visual_add_pile(s->visual, src1);

	src2 = vis_pile_create(i->src2);
	src2->origin[1] = 0 - (CARD_HEIGHT + CARD_SPACING);
	visual_add_pile(s->visual, src2);

	src3 = vis_pile_create(i->src3);
	src3->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING + CARD_HEIGHT / 2);
	src3->rotation = 90.0f;
	visual_add_pile(s->visual, src3);

	src4 = vis_pile_create(i->src4);
	src4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING + CARD_HEIGHT / 2;
	src4->rotation = 90.0f;
	visual_add_pile(s->visual, src4);

	build1 = vis_pile_create(i->build1);
	build1->origin[0] = 0 - (CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT);
	visual_add_pile(s->visual, build1);

	build2 = vis_pile_create(i->build2);
	build2->origin[0] = CARD_WIDTH + CARD_SPACING * 2 + CARD_HEIGHT;
	visual_add_pile(s->visual, build2);

	build3 = vis_pile_create(i->build3);
	build3->origin[1] = CARD_HEIGHT / 2 + CARD_SPACING * 2 + CARD_HEIGHT + CARD_WIDTH / 2;
	build3->rotation = 90.0f;
	visual_add_pile(s->visual, build3);

	build4 = vis_pile_create(i->build4);
	build4->origin[1] = 0 - (CARD_HEIGHT / 2 + CARD_SPACING * 2 + CARD_HEIGHT + CARD_WIDTH / 2);
	build4->rotation = 90.0f;
	visual_add_pile(s->visual, build4);

	pile1 = vis_pile_create(i->pile1);
	pile1->origin[0] = 0;
	pile1->origin[1] = 0;
	visual_add_pile(s->visual, pile1);

	pile2 = vis_pile_create(i->pile2);
	pile2->origin[0] = 0;
	pile2->origin[1] = 0;
	visual_add_pile(s->visual, pile2);

	pile3 = vis_pile_create(i->pile3);
	pile3->origin[0] = 0;
	pile3->origin[1] = 0;
	visual_add_pile(s->visual, pile3);

	pile4 = vis_pile_create(i->pile4);
	pile4->origin[0] = 0;
	pile4->origin[1] = 0;
	visual_add_pile(s->visual, pile4);

	pile5 = vis_pile_create(i->pile5);
	pile5->origin[0] = 0;
	pile5->origin[1] = 0;
	visual_add_pile(s->visual, pile5);

	create_deck(i->deck);
	create_deck(i->deck);

	sync(s);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->move = my_move;
	s->free = my_free;
	return s;
}
