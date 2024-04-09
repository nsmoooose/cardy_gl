#pragma once

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
