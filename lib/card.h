#ifndef __CARD_H__
#define __CARD_H__

#include "memory.h"

#define CARD_WIDTH 40.0
#define CARD_HEIGHT 60.0
#define CARD_SPACING 4.0
#define CARD_THICKNESS 0.4

typedef int bool;
const int true;
const int false;

typedef enum {
	e_suit_none = -1,
	e_suit_first = 0,
	e_clubs = e_suit_first,
	e_diamonds = 1,
	e_hearts = 2,
	e_spades = 3,
	e_suit_last = e_spades
} card_suit;

typedef unsigned char card_value;

struct card_proxy_St;

/** This is information about a single card.
 */
typedef struct {
	card_value value;
	card_suit suit;

	/* A pointer to the proxy object. This is the object
	   that is known to the client app that renders this game.
	   This was way we can prevent the user from peeking at cards
	   that hasn't been revealed yet. */
	struct card_proxy_St* proxy;
} card;

/** A pile of cards. This is the internal representation of
 *  cards. No cards are hidden for the user and no visual
 *  state is held here.
 */
typedef struct {
	card** cards;
	unsigned int size;
} pile;

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
typedef struct visual_pile_St {
	card_proxy** cards;
	unsigned int card_count;

	/** The origin of this pile. Where it should be placed on the desk.
	 */
	float origin[3];

	/** Rotation of the pile of cards. All cards rendered on this pile
	 *  will have the same rotation.
	 */
	float rotation;

	/** Translation for every card.
	 */
	float translateX;
	float translateY;

	void (*pile_action)(struct solitaire_St* sol, struct visual_pile_St* pile);
	void (*card_action)(struct solitaire_St* sol, struct visual_pile_St* pile, card_proxy* proxy);

	void *data;
} visual_pile;

/** Defines some common settings used to calculate positions of piles
 *  and cards.
 */
typedef struct {
	float card_width;
	float card_height;
	float card_spacing;
	float card_thickness;
} visual_settings;

/** Represents the visual presentation of a game. Together
 *  with the pile struct it describes how to render the user
 *  interface.
 */
typedef struct {
	visual_pile** piles;
	int pile_count;

	visual_settings* settings;
} visual;

/** Interface with a solitaire game. All members of this
 *  struct is callbacks to the logic.
 */
typedef struct solitaire_St {
	/** Start a new game.
	 */
	void (*new_game)(struct solitaire_St *sol);

	/** Move card between two positions.
	 */
	bool (*append_to_pile)(struct solitaire_St *sol, visual_pile *dest, card_proxy *card);

	/** Event that is called when a card has been revealed
	 *  or hidden beqause of user action.
	 */
	void (*card_revealed)(struct solitaire_St *sol);

	/** The solitaire has been solved.
	 */
	void (*finished)(struct solitaire_St *sol);

	visual *visual;

	/** Internal representation of this game. Don't mess
	 *  with this one.
	 */
	void *data;
} solitaire;

/** Creates a single card with the suit and value.
 */
card* card_create(mem_context *context, card_suit suit, card_value value);

/** Frees the memory used by the card.
 */
void card_free(mem_context *context, card* card);

/** Creates a deck of cards with 52 cards if the array permits it.
 */
void create_deck(mem_context *context, pile *pile);

/** Print information about the solitaire. Usefull for debugging purposes
 *  when we need to know the content of the solitaire.
 */
void print_solitaire_info(solitaire* sol);

/** Counts the number of cards that exists within the array.
 */
int card_count(pile *pile);

card *card_take(pile *pile, int index);

/** Takes the last card from the array and returns it. The index that
 *  The card was found on will be cleared. This makes the card ready to
 *  be inserted into any other array.
 */
card* card_take_last(pile *pile);

/** Appends the card to the first free position in the array of cards.
 */
void card_append(card* card_to_append, pile *pile);

/** Moves all cards from src array into the destination array.
 */
void card_move_all(pile *dest, pile *src);
void card_move_count(pile *dest, pile *src, int count);

/** Creates a pile with the specified fixed size.
 */
pile* pile_create(mem_context *context, int size);

/** Returns the first free position of a card in the array.
 */
int card_first_free(pile *pile);

/** Returns the last (top card) in a pile.
 */
card *card_last(pile *pile);

/** Reveals the card to the user.
 */
void card_reveal(card *card);
void card_reveal_count(pile *pile, int start_index, int count);
void card_reveal_all(pile *pile);

void card_hide(card *card);
void card_hide_count(pile *pile, int start_index, int count);
void card_hide_all(pile *pile);

visual* visual_create(mem_context *context, visual_settings *settings);
void visual_add_pile(mem_context *context, visual *vis, visual_pile *p);
void visual_sync(visual *vis);

visual_pile* visual_pile_create(mem_context *context, pile *pile);

#endif /* __CARD_H__ */
