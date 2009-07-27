#ifndef __CARD_H__
#define __CARD_H__

#define CARD_WIDTH 40.0
#define CARD_HEIGHT 60.0
#define CARD_SPACING 4.0
#define CARD_THICKNESS 0.4

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

	void* data;
} card;

/** This is a placeholder for a single card. If the card
 *  hasn't been revealed yet the card pointer is null.
 */
typedef struct {
	card* card;
} card_proxy;

struct solitaire_St;

/** Contains information about a pile. All this information
 *  is then used by the rendering engine to visualize the
 *  content.
 */
typedef struct pile_St {
	card_proxy** first;
	unsigned int card_count;

	float origin[3];
	float rotation;
	float translateX;
	float translateY;

	void (*pile_action)(struct solitaire_St* sol, struct pile_St* pile);
	void (*card_action)(struct solitaire_St* sol, struct pile_St* pile, card_proxy* proxy);
} pile;

/** Interface with a solitaire game. All members of this
 *  struct is callbacks to the logic.
 */
typedef struct solitaire_St {
	/** Start a new game.
	 */
	void (*new_game)(struct solitaire_St* sol);

	/** Returns the number of piles available to display.
	 */
	int (*get_pile_count)(struct solitaire_St* sol);

	/** Returns information about a pile and a list of all
	 *  the cards in it.
	 */
	pile* (*get_pile)(struct solitaire_St* sol, int no);

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

extern solitaire* g_solitaire;

/** Creates a deck of cards with 52 cards if the array permits it.
 */
void create_deck(card* list[], int count);

/** Print information about the solitaire. Usefull for debugging purposes
 *  when we need to know the content of the solitaire.
 */
void print_solitaire_info(solitaire* sol);

/** Counts the number of cards that exists within the array.
 */
int card_count(card* cards[], int size);

/** Takes the last card from the array and returns it. The index that
 *  The card was found on will be cleared. This makes the card ready to
 *  be inserted into any other array.
 */
card* card_take_last(card* cards[], int size);

/** Appends the card to the first free position in the array of cards.
 */
void card_append(card* card_to_append, card* cards[], int size);

/** Moves all cards from src array into the destination array.
 */
void card_append_all(card* dest[], int dest_size, card* src[], int src_size);

/** Returns the first free position of a card in the array.
 */
int card_first_free(card* cards[], int size);

/** Reveals the card to the user.
 */
void card_reveal(card* card);

#endif /* __CARD_H__ */
