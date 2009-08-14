#include <stdlib.h>
#include "rule.h"
#include "solitaire_theidiot.h"

typedef struct {
	pile *deck;
	pile *pile1, *pile2, *pile3, *pile4;
	pile *done;
	ruleset *ruleset;
} internal;

static void my_new_game(solitaire* sol) {
}

static bool my_append_to_pile(solitaire *sol, vis_pile *dest, card_proxy *card) {
	bool result;
	move_action *action;
	internal* i = sol->data;

	action = get_move_action(sol->visual, card, dest);
	result = ruleset_check(i->ruleset, action);
	free(action);

	visual_sync(sol->visual);
	return result;
}

static void my_deal(solitaire* sol, vis_pile* pile) {
	internal* i = sol->data;

	if(card_count(i->deck) >= 4) {
		card* card1 = card_take_last(i->deck);
		card* card2 = card_take_last(i->deck);
		card* card3 = card_take_last(i->deck);
		card* card4 = card_take_last(i->deck);

		card_reveal(card1);
		card_reveal(card2);
		card_reveal(card3);
		card_reveal(card4);

		card_append(card1, i->pile1);
		card_append(card2, i->pile2);
		card_append(card3, i->pile3);
		card_append(card4, i->pile4);
	}

	visual_sync(sol->visual);
}

static void my_free(solitaire* sol) {
	internal* i = sol->data;

	pile_free(i->deck);
	pile_free(i->pile1);
	pile_free(i->pile2);
	pile_free(i->pile3);
	pile_free(i->pile4);
	pile_free(i->done);

	free(i);
	free(sol);
}

solitaire* solitaire_theidiot() {
	vis_pile *deck, *pile1, *pile2, *pile3, *pile4, *done;
	rule *rule1, *rule2;
	condition *pile1_4_cond;

	/* The one solitaire instance we have.*/
	solitaire* s = calloc(1, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = calloc(1, sizeof(internal));
	s->data = i;

	s->visual = visual_create();

	i->deck = pile_create(52);
	deck = vis_pile_create(i->deck);
	deck->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2);
	deck->origin[1] = 40.0f;
	deck->rotation = 45.0f;
	deck->pile_action = my_deal;
	visual_add_pile(s->visual, deck);

	i->pile1 = pile_create(13);
	pile1 = vis_pile_create(i->pile1);
	pile1->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING);
	pile1->origin[1] = 70.0f;
	pile1->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile1);

	i->pile2 = pile_create(13);
	pile2 = vis_pile_create(i->pile2);
	pile2->origin[0] = 0 - (CARD_WIDTH / 2 + CARD_SPACING / 2);
	pile2->origin[1] = 70.0f;
	pile2->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile2);

	i->pile3 = pile_create(13);
	pile3 = vis_pile_create(i->pile3);
	pile3->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2;
	pile3->origin[1] = 70.0f;
	pile3->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile3);

	i->pile4 = pile_create(13);
	pile4 = vis_pile_create(i->pile4);
	pile4->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH + CARD_SPACING;
	pile4->origin[1] = 70.0f;
	pile4->translateY = 0 - CARD_HEIGHT / 5;
	visual_add_pile(s->visual, pile4);

	i->done = pile_create(48);
	done = vis_pile_create(i->done);
	done->origin[0] = CARD_WIDTH / 2 + CARD_SPACING / 2 + CARD_WIDTH * 2 + CARD_SPACING * 2 + CARD_WIDTH / 2;
	done->origin[1] = 40.0f;
	done->rotation = -45.0f;
	visual_add_pile(s->visual, done);

	create_deck(i->deck);

	visual_sync(s->visual);

	i->ruleset = create_ruleset();

	/* Shared condition between several rules. */
	pile1_4_cond =
		condition_or(
			condition_or(
				condition_or(
					condition_source(i->pile1),
					condition_source(i->pile2)),
				condition_source(i->pile3)),
			condition_source(i->pile4));

	/* Move card to done pile if source is pile1-pile4 and there is
	   a higher card in same suit in those piles. */
	rule1 = create_rule();
	rule_add_condition(rule1, pile1_4_cond);
	rule_add_condition(rule1, condition_destination(i->done));
	ruleset_add_rule(i->ruleset, rule1);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	s->append_to_pile = my_append_to_pile;
	s->free = my_free;
	return s;
}

#if 0
void ruleset_stub() {
	pile *pile1, *pile2, *pile3, *pile4, *done;
	rulebook *rulebook;
	rule *rule1, *rule2;
	condition *pile1_4_cond;

	ruleset = create_ruleset();

	/* Shared condition between several rules. */
	pile1_4_cond =
		condition_or(
			condition_or(
				condition_or(
					condition_source(pile1),
					condition_source(pile2)),
				condition_source(pile3)),
			condition_source(pile4))

	/* Move card to done pile if source is pile1-pile4 and there is
	   a higher card in same suit in those piles. */
	rule1 = create_rule();
	rule_add_condition(rule1, pile1_4_cond);
	rule_add_condition(rule1, condition_top_card());
	rule_add_condition(rule1, condition_destination(done));
	rule_add_condition(
		rule1,
		condition_or(
			condition_or(
				condition_or(
					condition_top_card_compare(pile1, e_higher, e_follow_suit),
					condition_top_card_compare(pile2, e_higher, e_follow_suit)),
				condition_top_card_compare(pile3, e_higher, e_follow_suit)),
			condition_top_card_compare(pile4, e_higher, e_follow_suit))
		);
	ruleset_add_rule(ruleset, rule1);

	/* Allow move of cards if pile is empty. */
	rule2 = create_rule();
	rule_add_condition(rule2, pile1_4_cond);
	rule_add_condition(rule2, condition_top_card());
	rule_add_condition(rule2, condition_destination_empty());
	ruleset_add_rule(ruleset, rule2);

	ruleset_check(ruleset, action);
}
#endif
