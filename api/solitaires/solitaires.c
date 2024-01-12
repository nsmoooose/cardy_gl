#include "solitaires.h"
#include "theidiot.h"
#include "maltesercross.h"
#include "noname1.h"
#include "pyramid.h"
#include "heirship.h"
#include "test1.h"

game_registry *solitaire_get_registry(void) {
	game_registry *r = game_registry_create();
	game_registry_add(r, "idiot", game_create("The idiot", false, solitaire_theidiot));
	game_registry_add(r, "malteser_cross",
	                  game_create("Malteser cross", false, solitaire_maltesercross));
	game_registry_add(r, "noname1", game_create("Noname1", false, solitaire_noname1));
	game_registry_add(r, "pyramid", game_create("Pyramid", true, solitaire_pyramid));
	game_registry_add(r, "heirship",
	                  game_create("Heirship", true, solitaire_heirship));
	game_registry_add(r, "test1", game_create("Test1", true, solitaire_test1));
	return r;
}
