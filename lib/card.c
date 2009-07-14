#include <stdlib.h>
#include <stdio.h>
#include "card.h"

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
