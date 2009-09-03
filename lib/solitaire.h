#ifndef __SOLITAIRE_H__
#define __SOLITAIRE_H__

#include "rule.h"

/** Interface with a solitaire game. All members of this
 *  struct is callbacks to the logic.
 */
typedef struct solitaire_St {
	/** Start a new game.
	 */
	void (*new_game)(struct solitaire_St *sol);

	/** Event that is called when a card has been revealed
	 *  or hidden beqause of user action.
	 */
	void (*card_revealed)(struct solitaire_St *sol);

	/** The solitaire has been solved.
	 */
	void (*finished)(struct solitaire_St *sol);

	visual *visual;

	ruleset *ruleset;

	/** Internal representation of this game. Don't mess
	 *  with this one.
	 */
	void *data;
} solitaire;

/** Print information about the solitaire. Usefull for debugging purposes
 *  when we need to know the content of the solitaire.
 */
void print_solitaire_info(solitaire* sol);

#endif /* __SOLITAIRE_H__ */
