#include <stdlib.h>
#include "rule.h"
#include "solitaire_maltesercross.h"

typedef struct {
	pile* deck;

	/* The four piles where we draw cards from. */
	pile* ace1;
	pile* ace2;
	pile* ace3;
	pile* ace4;

	/* The four diagonal piles where you build
	 * from king to ace. */
	pile* pile1;
	pile* pile2;
	pile* pile3;
	pile* pile4;
	pile* pile5;
	pile* pile6;
	pile* pile7;
	pile* pile8;
} internal;

typedef struct {
	solitaire *sol;
	internal *i;
} action_deal_data;

static void action_deal_execute(visual_pile_action *action) {
	action_deal_data *data = action->data;
	internal *i = data->i;
	solitaire *sol = data->sol;

	if(card_count(i->deck) == 52) {
		card_move_count(i->pile1, i->deck, 7);
		card_reveal_count(i->pile1, 4, 3);

		card_move_count(i->pile2, i->deck, 7);
		card_reveal_count(i->pile2, 4, 3);

		card_move_count(i->pile3, i->deck, 7);
		card_reveal_count(i->pile3, 4, 3);

		card_move_count(i->pile4, i->deck, 7);
		card_reveal_count(i->pile4, 4, 3);

		card_move_count(i->pile5, i->deck, 6);
		card_reveal_all(i->pile5);

		card_move_count(i->pile6, i->deck, 6);
		card_reveal_all(i->pile6);

		card_move_count(i->pile7, i->deck, 6);
		card_reveal_all(i->pile7);

		card_move_count(i->pile8, i->deck, 6);
		card_reveal_all(i->pile8);
	}
	else {
		card_move_all(i->deck, i->pile1);
		card_move_all(i->deck, i->pile2);
		card_move_all(i->deck, i->pile3);
		card_move_all(i->deck, i->pile4);
		card_move_all(i->deck, i->pile5);
		card_move_all(i->deck, i->pile6);
		card_move_all(i->deck, i->pile7);
		card_move_all(i->deck, i->pile8);

		card_move_all(i->deck, i->ace1);
		card_move_all(i->deck, i->ace2);
		card_move_all(i->deck, i->ace3);
		card_move_all(i->deck, i->ace4);

		card_hide_all(i->deck);
	}

	visual_sync(sol->visual);
}

static visual_pile_action *action_deal(mem_context *context, solitaire *sol, internal *i) {
	visual_pile_action *deal_action = mem_alloc(context, sizeof(visual_pile_action));
	action_deal_data *data = mem_alloc(context, sizeof(action_deal_data));
	data->sol = sol;
	data->i = i;
	deal_action->data = data;
	deal_action->execute = action_deal_execute;
	return deal_action;
}

solitaire* solitaire_noname1(mem_context *context, visual_settings *settings) {
	visual_pile *deck, *ace1, *ace2, *ace3, *ace4;
	visual_pile *pile1, *pile2, *pile3, *pile4, *pile5, *pile6, *pile7, *pile8;
	condition *ace1_4_cond;
	rule *rule1, *rule2, *rule3;

	/* The one solitaire instance we have.*/
	solitaire* s = mem_alloc(context, sizeof(solitaire));

	/* This is the internal data representation of this
	 * solitaire. This is a local struct hidden from other
	 * members. */
	internal* i = mem_alloc(context, sizeof(internal));
	s->data = i;
	s->visual = visual_create(context, settings);

	i->deck = pile_create(context, 52);
	i->ace1 = pile_create(context, 13);
	i->ace2 = pile_create(context, 13);
	i->ace3 = pile_create(context, 13);
	i->ace4 = pile_create(context, 13);
	i->pile1 = pile_create(context, 52);
	i->pile2 = pile_create(context, 52);
	i->pile3 = pile_create(context, 52);
	i->pile4 = pile_create(context, 52);
	i->pile5 = pile_create(context, 52);
	i->pile6 = pile_create(context, 52);
	i->pile7 = pile_create(context, 52);
	i->pile8 = pile_create(context, 52);

	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2 + settings->card_width / 2);
	deck->origin[1] = 70.0f;
	deck->rotation = 45.0f;
	deck->action = action_deal(context, s, i);
	visual_add_pile(context, s->visual, deck);

	ace1 = visual_pile_create(context, i->ace1);
	ace1->origin[0] = settings->card_width / 2 + settings->card_spacing / 2;
	ace1->origin[1] = 70.0f;
	visual_add_pile(context, s->visual, ace1);

	ace2 = visual_pile_create(context, i->ace2);
	ace2->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing;
	ace2->origin[1] = 70.0f;
	visual_add_pile(context, s->visual, ace2);

	ace3 = visual_pile_create(context, i->ace3);
	ace3->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2;
	ace3->origin[1] = 70.0f;
	visual_add_pile(context, s->visual, ace3);

	ace4 = visual_pile_create(context, i->ace4);
	ace4->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 3 + settings->card_spacing * 3;
	ace4->origin[1] = 70.0f;
	visual_add_pile(context, s->visual, ace4);

	pile1 = visual_pile_create(context, i->pile1);
	pile1->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 3 + settings->card_spacing * 3);
	pile1->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile1);

	pile2 = visual_pile_create(context, i->pile2);
	pile2->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2);
	pile2->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile2);

	pile3 = visual_pile_create(context, i->pile3);
	pile3->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing);
	pile3->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile3);

	pile4 = visual_pile_create(context, i->pile4);
	pile4->origin[0] = 0 - (settings->card_width / 2 + settings->card_spacing / 2);
	pile4->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile4);

	pile5 = visual_pile_create(context, i->pile5);
	pile5->origin[0] = settings->card_width / 2 + settings->card_spacing / 2;
	pile5->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile5);

	pile6 = visual_pile_create(context, i->pile6);
	pile6->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width + settings->card_spacing;
	pile6->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile6);

	pile7 = visual_pile_create(context, i->pile7);
	pile7->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 2 + settings->card_spacing * 2;
	pile7->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile7);

	pile8 = visual_pile_create(context, i->pile8);
	pile8->origin[0] = settings->card_width / 2 + settings->card_spacing / 2 + settings->card_width * 3 + settings->card_spacing * 3;
	pile8->translateY = 0 - settings->card_height / 5;
	visual_add_pile(context, s->visual, pile8);

	create_deck(context, i->deck, 1);

	card_shuffle(i->deck);

	visual_sync(s->visual);

	s->ruleset = ruleset_create(context);

	/* Shared condition for the aces. */
	ace1_4_cond =
		condition_or(
			context,
			condition_or(
				context,
				condition_or(
					context,
					condition_destination(context, i->ace1),
					condition_destination(context, i->ace2)),
				condition_destination(context, i->ace3)),
			condition_destination(context, i->ace4));

	/* Allow move of ace to an empty pile among the ace piles. */
	rule1 = rule_create(context);
	rule_add_condition(context, rule1, ace1_4_cond);
	rule_add_condition(context, rule1, condition_destination_empty(context));
	rule_add_condition(context, rule1, condition_top_card(context));
	rule_add_condition(context, rule1, condition_top_card_equal(context, e_suit_none, 1, e_equal_value));
	rule_add_action(context, rule1, action_reveal_source_top_card(context));
	ruleset_add_rule(context, s->ruleset, rule1);

	/* Allow card to be placed on top of the ace
	   piles following suit and ascending order. */
	rule2 = rule_create(context);
	rule_add_condition(context, rule2, ace1_4_cond);
	rule_add_condition(context, rule2, condition_top_card(context));
	rule_add_condition(context, rule2, condition_top_card_compare(context, 0, e_dest_1lower_value|e_follow_suit));
	ruleset_add_rule(context, s->ruleset, rule2);

	/* Allow moving several cards to a top card of the opposite suit. */
	rule3 = rule_create(context);
	rule_add_condition(context, rule2, condition_top_card_compare(context, 0, e_dest_1higher_value|e_suit_opposite));
	ruleset_add_rule(context, s->ruleset, rule3);
	return s;
}
