#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "card.h"

const int true = 1;
const int false = 0;

card* card_create(mem_context *context, card_suit suit, card_value value) {
	card* c = mem_alloc(context, sizeof(card));
	c->value = value;
	c->suit = suit;
	c->proxy = mem_alloc(context, sizeof(card_proxy));
	return c;
}

void card_free(mem_context *context, card* card) {
	if(card) {
		mem_free(context, card->proxy);
		mem_free(context, card);
	}
}

void create_deck(mem_context *context, pile *pile, card_value ace) {
	int index = 0;
	card* card;
	card_suit suit;
	card_value value;

	for(suit=e_suit_first;suit<=e_suit_last;++suit) {
		for(value=1;value<14;++value, ++index) {
			card = card_create(context, suit, value);
			if(value == 1) {
				card->value = ace;
			}
			card_append(card, pile);
		}
	}
}

int card_count(pile *pile) {
	int count = 0, index = 0;
	for(;index<pile->size;++index) {
		if(pile->cards[index]) {
			count++;
		}
	}
	return count;
}

card *card_take(pile *pile, int index) {
	int i;
	card *c = pile->cards[index];
	pile->cards[index] = 0;
	for(i=index+1;i<pile->size;++i) {
		pile->cards[i-1] = pile->cards[i];
		pile->cards[i] = 0;
	}
	return c;
}

card* card_take_last(pile *pile) {
	card* last = 0;
	int index = 0, last_index = -1;
	for(;index<pile->size;++index) {
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

card *card_last(pile *pile) {
	int index;
	for(index = pile->size-1;index >= 0;--index) {
		if(pile->cards[index]) {
			return pile->cards[index];
		}
	}
	return 0;
}

void card_append(card *card_to_append, pile *pile) {
	int free_index = card_first_free(pile);
	pile->cards[free_index] = card_to_append;
}

int card_first_free(pile *pile) {
	int index=0;
	for(;index<pile->size;++index) {
		if(pile->cards[index]==0) {
			return index;
		}
	}
	return -1;
}

void card_move_all(pile *dest, pile *src) {
	int index, dest_index;
	for(index=0;index<src->size;++index) {
		if(!src->cards[index]) {
			continue;
		}

		dest_index = card_first_free(dest);
		dest->cards[dest_index] = src->cards[index];
		src->cards[index] = 0;
	}
}

void card_move_all_array(pile *dest, int count, ...) {
	int index;
	va_list vl;

	va_start(vl, count);
	for(index=0;index<count;++index) {
		card_move_all(dest, va_arg(vl, pile*));
	}
	va_end(vl);
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
		if(index >= pile->size) {
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
	for(index=0;index<pile->size;++index) {
		if(pile->cards[index]) {
			card_reveal(pile->cards[index]);
		}
	}
}

void card_reveal_all_array(int count, ...) {
	int index;
	va_list vl;

	va_start(vl, count);
	for(index=0;index<count;++index) {
		card_reveal_all(va_arg(vl, pile*));
	}
	va_end(vl);
}

void card_hide(card *card) {
	card->proxy->card = 0;
}

void card_hide_count(pile *pile, int start_index, int count) {
	int index;
	for(index=start_index;index<start_index+count;++index) {
		if(index >= pile->size) {
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
	for(index=0;index<pile->size;++index) {
		if(pile->cards[index]) {
			card_hide(pile->cards[index]);
		}
	}
}

void card_shuffle(pile *pile) {
	card *tmp;
	int i1, i2, i = 0, count = card_count(pile);
	if(count == 0) {
		return;
	}

	for(i=0;i<count*4;++i) {
		i1 = rand() % count;
		i2 = rand() % count;
		tmp = pile->cards[i1];
		pile->cards[i1] = pile->cards[i2];
		pile->cards[i2] = tmp;
	}
}

pile* pile_create(mem_context *context, int size) {
	pile* p = mem_alloc(context, sizeof(pile));
	p->cards = mem_alloc(context, size * sizeof(pile*));
	p->size = size;
	return p;
}

visual* visual_create(mem_context *context, visual_settings *settings) {
	visual *vis = (visual*)mem_alloc(context, sizeof(visual));
	vis->settings = settings;
	return vis;
}

void visual_sync(visual *vis) {
	int pile_index, card_index;
	pile *src;
	visual_pile *dst;

	for(pile_index=0;pile_index<vis->pile_count;++pile_index) {
		dst = vis->piles[pile_index];
		src = (pile*)dst->data;

		dst->card_count = 0;
		for(card_index=0;card_index<src->size;++card_index) {
			if(src->cards[card_index]) {
				dst->cards[card_index] = src->cards[card_index]->proxy;
				dst->card_count++;
			}
			else {
				dst->cards[card_index] = 0;
			}
		}
	}
}

void visual_add_pile(mem_context *context, visual* vis, visual_pile* p) {
	visual_pile** old_piles  = vis->piles;
	vis->piles = mem_alloc(context, (vis->pile_count + 1) * sizeof(visual_pile*));
	vis->pile_count++;

	if(old_piles) {
		memcpy(vis->piles, old_piles, sizeof(visual_pile*) * (vis->pile_count - 1));
		mem_free(context, old_piles);
	}
	vis->piles[vis->pile_count - 1] = p;
}

visual_pile *visual_find_pile_from_card(visual *vis, card_proxy *proxy) {
	int i, j;
	for(i=0;i<vis->pile_count;++i) {
		for(j=0;j<vis->piles[i]->card_count;++j) {
			if(vis->piles[i]->cards[j] == proxy) {
				return vis->piles[i];
			}
		}
	}
	return 0;
}

int visual_get_card_index(visual_pile *pile, card_proxy *card) {
	int i;
	for(i=0;i<pile->card_count;++i) {
		if(pile->cards[i] == card) {
			return i;
		}
	}
	return -1;
}

int visual_get_rest_of_pile(visual *vis, card_proxy *card) {
	visual_pile *pile = visual_find_pile_from_card(vis, card);
	int index = visual_get_card_index(pile, card);
	return pile->card_count - index;
}

visual_pile* visual_pile_create(mem_context *context, pile *pile) {
	visual_pile* p = mem_alloc(context, sizeof(visual_pile));
	p->data = pile;
	p->cards = mem_alloc(context, pile->size * sizeof(card_proxy*));
	return p;
}
