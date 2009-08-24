#include <stdio.h>
#include "lib/card.h"

void print_solitaire_info(solitaire* sol) {
	int i, j;
	visual_pile *pile;

	for(i=0;i<sol->visual->pile_count;++i) {
		printf("Pile: %d\n", i);
		pile = sol->visual->piles[i];
		for(j=0;j<pile->card_count;++j) {
			if(pile->cards[j]->card == 0) {
				printf("Card at index: %d is facing down.\n", j);
			}
			else {
				printf("TODO");
			}
		}
	}
}
