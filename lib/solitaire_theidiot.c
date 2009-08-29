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

static bool my_append_to_pile(solitaire *sol, visual_pile *dest, card_proxy *card) {
	bool result;
	move_action *action;
	internal* i = sol->data;

	action = get_move_action(sol->visual, card, dest);
	result = ruleset_check(i->ruleset, action);
	if(result) {
		apply_move_action(sol->visual, action);
	}
	free(action);

	visual_sync(sol->visual);
	return result;
}

static void my_deal(solitaire* sol, visual_pile* pile) {
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

solitaire* solitaire_theidiot(mem_context *context, visual_settings *settings) {
	visual_pile *deck, *pile1, *pile2, *pile3, *pile4, *done;
	rule *rule1, *rule2;
	condition *pile1_4_cond;

	/* The one solitaire instance we have.*/
	solitaire* s = mem_alloc(context, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = mem_alloc(context, sizeof(internal));
	s->data = i;

	s->visual = visual_create(context, settings);

	i->deck = pile_create(context, 52);
	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2 + settings->card_width / 2);
	deck->origin[1] = 40.0f;
	deck->rotation = 45.0f;
	deck->pile_action = my_deal;
	visual_add_pile(context, s->visual, deck);

	i->pile1 = pile_create(context, 13);
	pile1 = visual_pile_create(context, i->pile1);
	pile1->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing);
	pile1->origin[1] = 70.0f;
	pile1->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile1);

	i->pile2 = pile_create(context, 13);
	pile2 = visual_pile_create(context, i->pile2);
	pile2->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2);
	pile2->origin[1] = 70.0f;
	pile2->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile2);

	i->pile3 = pile_create(context, 13);
	pile3 = visual_pile_create(context, i->pile3);
	pile3->origin[0] = settings->card_width / 2 + settings->card_spacing / 2;
	pile3->origin[1] = 70.0f;
	pile3->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile3);

	i->pile4 = pile_create(context, 13);
	pile4 = visual_pile_create(context, i->pile4);
	pile4->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing;
	pile4->origin[1] = 70.0f;
	pile4->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile4);

	i->done = pile_create(context, 48);
	done = visual_pile_create(context, i->done);
	done->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2 + settings->card_width / 2;
	done->origin[1] = 40.0f;
	done->rotation = -45.0f;
	visual_add_pile(context, s->visual, done);

	create_deck(context, i->deck);

	visual_sync(s->visual);

	i->ruleset = create_ruleset(context);

	/* Shared condition between several rules. */
	pile1_4_cond =
		condition_or(
			context,
			condition_or(
				context,
				condition_or(
					context,
					condition_source(context, i->pile1),
					condition_source(context, i->pile2)),
				condition_source(context, i->pile3)),
			condition_source(context, i->pile4));

	/* Move card to done pile if source is pile1-pile4 and there is
	   a higher card in same suit in those piles. */
	rule1 = create_rule(context);
	rule_add_condition(context, rule1, pile1_4_cond);
	rule_add_condition(context, rule1, condition_destination(context, i->done));
	rule_add_condition(
		context,
		rule1,
		condition_or(
			context,
			condition_or(
				context,
				condition_or(
					context,
					condition_top_card_compare(context, i->pile1, e_dest_higher_value | e_follow_suit),
					condition_top_card_compare(context, i->pile2, e_dest_higher_value | e_follow_suit)),
				condition_top_card_compare(context, i->pile3, e_dest_higher_value | e_follow_suit)),
			condition_top_card_compare(context, i->pile4, e_dest_higher_value | e_follow_suit))
		);
	rule_add_condition(context, rule1, condition_top_card(context));
	ruleset_add_rule(context, i->ruleset, rule1);

	rule2 = create_rule(context);
	rule_add_condition(context, rule2, pile1_4_cond);
	rule_add_condition(context, rule2, condition_top_card(context));
	rule_add_condition(context, rule2, condition_destination_empty(context));
	rule_add_condition(
		context,
		rule2,
		condition_or(
			context,
			condition_or(
				context,
				condition_or(
					context,
					condition_destination(context, i->pile1),
					condition_destination(context, i->pile2)),
				condition_destination(context, i->pile3)),
			condition_destination(context, i->pile4)));
	ruleset_add_rule(context, i->ruleset, rule2);

	/* Add our implementation for the common functionality
	 * shared by all solitaires. */
	s->new_game = my_new_game;
	s->append_to_pile = my_append_to_pile;
	return s;
}
