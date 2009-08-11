#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "card.h"

/** Global instance of the one and only solitaire that is currently
 *  running.
 */
solitaire* g_solitaire = 0;

card* card_create(card_suit suit, card_value value) {
	card* card = calloc(1, sizeof(card));
	card->value = value;
	card->suit = suit;
	card->proxy = calloc(1, sizeof(card_proxy));
	return card;
}

void card_free(card* card) {
	if(card) {
		free(card->proxy);
		free(card);
	}
}

void create_deck(pile *pile) {
	int index = 0;
	card* card;
	card_suit suit;
	card_value value;

	for(suit=e_diamonds;suit<=e_spades;++suit) {
		for(value=1;value<14;++value, ++index) {
			card = card_create(suit, value);
			card_append(card, pile);
		}
	}
}

void print_solitaire_info(solitaire* sol) {
	int i, j;
	vis_pile *pile;

	for(i=0;i<sol->visual->pile_count;++i) {
		printf("Pile: %d\n", i);
		pile = sol->visual->piles[i];
		for(j=0;j<pile->card_count;++j) {
			if(pile->first[j]->card == 0) {
				printf("Card at index: %d is facing down.\n", j);
			}
			else {
				printf("TODO");
			}
		}
	}
}

int card_count(pile *pile) {
	int count = 0, index = 0;
	for(;index<pile->card_count;++index) {
		if(pile->cards[index]) {
			count++;
		}
	}
	return count;
}

card* card_take_last(pile *pile) {
	card* last = 0;
	int index = 0, last_index = -1;
	for(;index<pile->card_count;++index) {
		if(pile->cards[index]) {
			last = pile->cards[index];
			last_index = index;
		}
	}

	if(last_index != -1) {
		card* last = pile->cards[last_index];
		pile->cards[last_index] = 0;
		return last;
	}
	return 0;
}

void card_append(card* card_to_append, pile *pile) {
	int free_index = card_first_free(pile);
	pile->cards[free_index] = card_to_append;
}

int card_first_free(pile *pile) {
	int index=0;
	for(;index<pile->card_count;++index) {
		if(pile->cards[index]==0) {
			return index;
		}
	}
	return -1;
}

void card_move_all(pile *dest, pile *src) {
	int index, dest_index;
	for(index=0;index<src->card_count;++index) {
		if(!src->cards[index]) {
			continue;
		}

		dest_index = card_first_free(dest);
		dest->cards[dest_index] = src->cards[index];
		src->cards[index] = 0;
	}
}

void card_move_count(pile *dest, pile *src, int count) {
	int index;
	card* card;
	for(index=0;index<count;++index) {
		card = card_take_last(src);
		if(card) {
			card_append(card, dest);
		}
		else {
			/* TODO. This is an error that should be signalled or something. */
		}
	}
}

void card_reveal(card* card) {
	card->proxy->card = card;
}

void card_reveal_count(pile *pile, int start_index, int count) {
	int index;
	for(index=start_index;index<start_index+count;++index) {
		if(index >= pile->card_count) {
			/* TODO. This is an error. We should signal
			   in some way that you didn't handle the number
			   of cards correctly.
			*/
			return;
		}
		card_reveal(pile->cards[index]);
	}
}

void card_reveal_all(pile *pile) {
	int index;
	for(index=0;index<pile->card_count;++index) {
		if(pile->cards[index]) {
			card_reveal(pile->cards[index]);
		}
	}
}

void card_hide(card *card) {
	card->proxy->card = 0;
}

void card_hide_count(pile *pile, int start_index, int count) {
	int index;
	for(index=start_index;index<start_index+count;++index) {
		if(index >= pile->card_count) {
			/* TODO. This is an error. We should signal
			   in some way that you didn't handle the number
			   of cards correctly.
			*/
			return;
		}
		card_hide(pile->cards[index]);
	}
}

void card_hide_all(pile *pile) {
	int index;
	for(index=0;index<pile->card_count;++index) {
		if(pile->cards[index]) {
			card_hide(pile->cards[index]);
		}
	}
}

pile* pile_create(int size) {
	pile* p = calloc(1, sizeof(pile));
	p->cards = calloc(size, sizeof(pile*));
	p->card_count = size;
	return p;
}

void pile_free(pile* pile) {
	free(pile->cards);
	free(pile);
}

visual* visual_create() {
	visual* vis = calloc(1, sizeof(visual));
	return vis;
}

void visual_sync(visual *vis) {
	int pile_index, card_index;
	pile *src;
	vis_pile *dst;

	for(pile_index=0;pile_index<vis->pile_count;++pile_index) {
		dst = vis->piles[pile_index];
		src = (pile*)dst->data;

		dst->card_count = 0;
		for(card_index=0;card_index<src->card_count;++card_index) {
			if(src->cards[card_index]) {
				dst->first[card_index] = src->cards[card_index]->proxy;
				dst->card_count++;
			}
			else {
				dst->first[card_index] = 0;
			}
		}
	}
}

void visual_add_pile(visual* vis, vis_pile* p) {
	vis_pile** old_piles  = vis->piles;
	vis->piles = calloc(vis->pile_count + 1, sizeof(vis_pile*));
	vis->pile_count++;

	if(old_piles) {
		memcpy(vis->piles, old_piles, sizeof(vis_pile*) * (vis->pile_count - 1));
		free(old_piles);
	}
	vis->piles[vis->pile_count - 1] = p;
}

void visual_free(visual* vis) {
	int index;
	for(index=0;index<vis->pile_count;++index) {
		vis_pile_free(vis->piles[index]);
	}
	free(vis->piles);
	free(vis);
}

vis_pile* vis_pile_create(pile *pile) {
	vis_pile* p = calloc(1, sizeof(vis_pile));
	p->data = pile;
	p->first = calloc(pile->card_count, sizeof(card_proxy*));
	return p;
}

void vis_pile_free(vis_pile* pile) {
	free(pile->first);
	free(pile);
}
