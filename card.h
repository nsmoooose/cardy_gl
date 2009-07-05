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
typedef struct {
	/* Member functions about this game. Most of these
	 * are callbacks to the real hidden logic. */

	/* deal = deal the deck of cards. This means that
	 * a new solitaire is started.*/

	/* move card = move card between two positions. */

	/* card revealed|hidden = events that is called
	 * when a card has been revealed beqause of user
	 * action. */

	/* finished = the solitaire has been solved. */

	/* free = free all memory held by this solitaire. */

	/** Internal representation of this game. Don't mess
	 *  with this one.
	 */
	void* data;
} solitaire;

#endif // __CARD_H__
