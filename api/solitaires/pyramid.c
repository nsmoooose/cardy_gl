#include <stdlib.h>
#include "pyramid.h"

typedef struct {
	pile *piles[30];

	pile *deck;
	pile *done;
} internal;

/*
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
*/

solitaire* solitaire_pyramid(mem_context *context, visual_settings *settings) {
	int index;
	visual_pile *piles[30];

	/* The one solitaire instance we have.*/
	solitaire* s = mem_alloc(context, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = mem_alloc(context, sizeof(internal));
	s->data = i;
	s->visual = visual_create(context, settings);

	for(index=0;index<30;++index) {
		if(index == 0 || index == 29) {
			i->piles[index] = pile_create(context, 52);
		}
		else {
			i->piles[index] = pile_create(context, 1);
		}
		piles[index] = visual_pile_create(context, i->piles[index]);
		visual_add_pile(context, s->visual, piles[index]);
	}

	i->deck = i->piles[0];
	i->done = i->piles[29];

#if 0
	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2 + settings->card_width / 2);
	deck->origin[1] = 40.0f;
	deck->rotation = 45.0f;
	/*
	deck->pile_action = my_deal;
	*/
	visual_add_pile(context, s->visual, deck);

	pile1 = visual_pile_create(context, i->pile1);
	pile1->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing);
	pile1->origin[1] = 70.0f;
	pile1->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile1);

	pile2 = visual_pile_create(context, i->pile2);
	pile2->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2);
	pile2->origin[1] = 70.0f;
	pile2->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile2);

	pile3 = visual_pile_create(context, i->pile3);
	pile3->origin[0] = settings->card_width / 2 + settings->card_spacing / 2;
	pile3->origin[1] = 70.0f;
	pile3->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile3);

	pile4 = visual_pile_create(context, i->pile4);
	pile4->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing;
	pile4->origin[1] = 70.0f;
	pile4->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile4);

	done = visual_pile_create(context, i->done);
	done->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2 + settings->card_width / 2;
	done->origin[1] = 40.0f;
	done->rotation = -45.0f;
	visual_add_pile(context, s->visual, done);

	card_create_deck(context, i->deck, 1);

	ace = card_create(context, e_spades, 1);
	card_reveal(ace),
	card_append(ace, i->pile1);
#endif

	visual_sync(s->visual);
	return s;
}
