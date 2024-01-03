#ifndef __SOLITAIRE_H__
#define __SOLITAIRE_H__

#include "rule.h"

/** Interface with a solitaire game. All members of this
 *  struct is callbacks to the logic.
 */
typedef struct solitaire_St {
	visual *visual;
	ruleset *ruleset;

	/** Internal representation of this game. Don't mess
	 *  with this one.
	 */
	void *data;
} solitaire;

typedef solitaire *(*solitaire_create)(mem_context *context,
                                       visual_settings *settings);

/** Print information about the solitaire. Usefull for debugging purposes
 *  when we need to know the content of the solitaire.
 */
void print_solitaire_info(solitaire *sol);

#endif /* __SOLITAIRE_H__ */
