#include "solitaires.h"
#include "solitaire_theidiot.h"
#include "solitaire_maltesercross.h"
#include "solitaire_noname1.h"
#include "solitaire_pyramid.h"
#include "solitaire_heirship.h"
#include "solitaire_test1.h"

game_registry *solitaire_get_registry() {
	game_registry *r = game_registry_create();
	game_registry_add(r, "idiot", game_create("The idiot", solitaire_theidiot));
	game_registry_add(r, "malteser_cross", game_create("Malteser cross", solitaire_maltesercross));
	game_registry_add(r, "noname1", game_create("Noname1", solitaire_noname1));
	game_registry_add(r, "pyramid", game_create("Pyramid", solitaire_pyramid));
	game_registry_add(r, "heirship", game_create("Heirship", solitaire_heirship));
	game_registry_add(r, "test1", game_create("Test1", solitaire_test1));
	return r;
}
