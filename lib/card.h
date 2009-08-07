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

typedef unsigned char card_value;

struct card_proxy_St;

/** This is information about a single card.
 */
typedef struct {
	card_value value;
	card_suit suit;

	struct card_proxy_St* proxy;
} card;

/** This is a placeholder for a single card. If the card
 *  hasn't been revealed yet the card pointer is null.
 */
typedef struct card_proxy_St {
	card* card;
} card_proxy;

struct solitaire_St;

/** Contains information about a pile. All this information
 *  is then used by the rendering engine to visualize the
 *  content.
 */
typedef struct vis_pile_St {
	card_proxy** first;
	unsigned int card_count;

	float origin[3];
	float rotation;
	float translateX;
	float translateY;

	void (*pile_action)(struct solitaire_St* sol, struct vis_pile_St* pile);
	void (*card_action)(struct solitaire_St* sol, struct vis_pile_St* pile, card_proxy* proxy);
} vis_pile;

/** Represents the visual presentation of a game. Together
 *  with the pile struct it describes how to render the user
 *  interface.
 */
typedef struct {
	vis_pile** piles;
	int pile_count;
} visual;

/** Interface with a solitaire game. All members of this
 *  struct is callbacks to the logic.
 */
typedef struct solitaire_St {
	/** Start a new game.
	 */
	void (*new_game)(struct solitaire_St* sol);

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

	visual* visual;

	/** Internal representation of this game. Don't mess
	 *  with this one.
	 */
	void* data;
} solitaire;

extern solitaire* g_solitaire;

/** Creates a single card with the suit and value.
 */
card* card_create(card_suit suit, card_value value);

/** Frees the memory used by the card.
 */
void card_free(card* card);

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
void card_move_all(card* dest[], int dest_size, card* src[], int src_size);
void card_move_count(card* dest[], int dest_size, card* src[], int src_size, int count);


/** Returns the first free position of a card in the array.
 */
int card_first_free(card* cards[], int size);

/** Reveals the card to the user.
 */
void card_reveal(card* card);

visual* visual_create();
void visual_add_pile(visual* vis, vis_pile* p);
void visual_free(visual* vis);

vis_pile* pile_create(int size);
void pile_free(vis_pile* pile);

#endif /* __CARD_H__ */
