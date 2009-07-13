#include <stdlib.h>
#include "card.h"

void create_deck(card* list[], int count) {
	char index = 0;
	card_suit suit;
	char value;

	for(suit=e_diamonds;suit<e_spades;++suit) {
		for(value=0;value<13;++value) {
			if(index < count) {
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
