#include <stdlib.h>
#include <stdio.h>
#include "card.h"

/** Global instance of the one and only solitaire that is currently
 *  running.
 */
solitaire* g_solitaire = 0;

void create_deck(card* list[], int count) {
	char index = 0;
	card_suit suit;
	char value;

	for(suit=e_diamonds;suit<=e_spades;++suit) {
		for(value=1;value<14;++value) {
			if(index >= count) {
				return;
			}

			card* card = calloc(1, sizeof(card));
			card->value = value;
			card->suit = suit;

			card->data = calloc(1, sizeof(card_proxy));
			list[index] = card;

			index++;
		}
	}
}

void print_solitaire_info(solitaire* sol) {
	int i, j;
	for(i=0;i<sol->get_pile_count(sol);++i) {
		printf("Pile: %d\n", i);
		pile* pile = sol->get_pile(sol, i);
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

int card_count(card* cards[], int size) {
	int count = 0, index = 0;
	for(;index<size;++index) {
		if(cards[index]) {
			count++;
		}
	}
	return count;
}

card* card_take_last(card* cards[], int size) {
	card* last = 0;
	int index = 0, last_index = -1;
	for(;index<size;++index) {
		if(cards[index]) {
			last = cards[index];
			last_index = index;
		}
	}

	if(last_index != -1) {
		card* last = cards[last_index];
		cards[last_index] = 0;
		return last;
	}
	return 0;
}

void card_append(card* card_to_append, card* cards[], int size) {
	int free_index = card_first_free(cards, size);
	cards[free_index] = card_to_append;
}

int card_first_free(card* cards[], int size) {
	int index=0;
	for(;index<size;++index) {
		if(cards[index]==0) {
			return index;
		}
	}
	return -1;
}
