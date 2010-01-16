#include "solitaires.h"
#include "solitaire_theidiot.h"
#include "solitaire_maltesercross.h"
#include "solitaire_noname1.h"
#include "solitaire_pyramid.h"
#include "solitaire_heirship.h"
#include "solitaire_test1.h"

game_registry *solitaire_get_registry() {
	game_registry *r = game_registry_create();
	game_registry_add(game_create("The idiot", solitaire_theidiot));
	game_registry_add(game_create("Malteser cross", solitaire_maltesercross));
	game_registry_add(game_create("Noname1", solitaire_noname1));
	game_registry_add(game_create("Pyramid", solitaire_pyramid));
	game_registry_add(game_create("Heirship", solitaire_heirship));
	game_registry_add(game_create("Test1", solitaire_test1));
	return r;
}
