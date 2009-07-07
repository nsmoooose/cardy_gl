#ifndef __CARD_H__
#define __CARD_H__

typedef enum {
	e_diamonds,
	e_clubs,
	e_hearts,
	e_spades
} card_suit;

/** This is information about a single card.
 */
typedef struct {
	unsigned char value;
	card_suit suit;
} card;

/** This is a placeholder for a single card. If the card
 *  hasn't been revealed yet the card pointer is null.
 */
typedef struct {
	card* card;
} card_proxy;

typedef struct card_proxy_list_St {
	card_proxy* item;
	struct card_proxy_list_St* next;
} card_proxy_list;

typedef struct {
	card_proxy_list* cards;
} pile;

/** Interface with a solitaire game. All members of this
 *  struct is callbacks to the logic.
 */
typedef struct solitaire_St {
	/** Deal the deck of cards. This means that a new solitaire is
	 *  started.
	 */
	void (*deal)(struct solitaire_St* sol);

	/** Returns the number of piles available to display.
	 */
	int (*get_pile_count)(struct solitaire_St* sol);

	/** Returns information about a pile and a list of all
	 *  the cards in it.
	 */
	pile* (*get_pile)(struct solitaire_St* sol);

	/** Move card between two positions.
	 */
	void (*move)(struct solitaire_St* sol, card_proxy* card_proxy);

	/** Event that is called when a card has been revealed
	 *  or hidden beqause of user action.
	 */
	void (*card_revealed)(struct solitaire_St* sol);

	/** The solitaire has been solved.
	 */
	void (*finished)(struct solitaire_St* sol);

	/** Free all memory held by this solitaire.
	 */
	void (*free)(struct solitaire_St* sol);

	/** Internal representation of this game. Don't mess
	 *  with this one.
	 */
	void* data;
} solitaire;


void create_deck(card* list[], int count);

#endif // __CARD_H__
