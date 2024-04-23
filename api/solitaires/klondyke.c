#include "api/rule.h"
#include "klondyke.h"

typedef struct {
	pile *deal;
	pile *deck;
	pile *src;
	pile *build[7];
	pile *ace[4];
} internal;

typedef struct {
	solitaire *sol;
	internal *i;
} action_data;

static void action_deck_execute(visual_pile_action *action);

static void action_deal_execute(visual_pile_action *action) {
	action_data *data = action->data;
	internal *i = data->i;
	solitaire *sol = data->sol;

	if (card_count(i->deal) == 52) {
		card_move_all(i->deck, i->deal);
		action_deck_execute(action);
	} else {
		card_move_all_array(i->deal, 13, i->deck, i->src, i->build[0],
		                    i->build[1], i->build[2], i->build[3], i->build[4],
		                    i->build[5], i->build[6], i->ace[0], i->ace[1],
		                    i->ace[2], i->ace[3]);
		card_hide_all(i->deal);
		card_shuffle(i->deal);
	}

	visual_sync(sol->visual);
}

static visual_pile_action *action_deal(mem_context *context, solitaire *sol,
                                       internal *i) {
	visual_pile_action *action = mem_alloc(context, sizeof(visual_pile_action));
	action_data *data = mem_alloc(context, sizeof(action_data));
	data->sol = sol;
	data->i = i;
	action->data = data;
	action->execute = action_deal_execute;
	return action;
}

static void action_deck_execute(visual_pile_action *action) {
	action_data *data = action->data;
	internal *i = data->i;
	solitaire *sol = data->sol;

	if (card_count(i->deck) == 52) {
		card_move_count(i->build[0], i->deck, 1);
		card_reveal_count(i->build[0], 0, 1);

		card_move_count(i->build[1], i->deck, 2);
		card_reveal_count(i->build[1], 1, 1);

		card_move_count(i->build[2], i->deck, 3);
		card_reveal_count(i->build[2], 2, 1);

		card_move_count(i->build[3], i->deck, 4);
		card_reveal_count(i->build[3], 3, 1);

		card_move_count(i->build[4], i->deck, 5);
		card_reveal_count(i->build[4], 4, 1);

		card_move_count(i->build[5], i->deck, 6);
		card_reveal_count(i->build[5], 5, 1);

		card_move_count(i->build[6], i->deck, 7);
		card_reveal_count(i->build[6], 6, 1);
	} else {
		if (card_count(i->deck) == 0) {
			card_move_all(i->deck, i->src);
			card_hide_all(i->deck);
		} else {
			card_move_count(i->src, i->deck, 1);
			card_reveal_all(i->src);
		}
	}

	visual_sync(sol->visual);
}

static visual_pile_action *action_deck(mem_context *context, solitaire *sol,
                                       internal *i) {
	visual_pile_action *action = mem_alloc(context, sizeof(visual_pile_action));
	action_data *data = mem_alloc(context, sizeof(action_data));
	data->sol = sol;
	data->i = i;
	action->data = data;
	action->execute = action_deck_execute;
	return action;
}

solitaire *solitaire_klondyke(mem_context *context, visual_settings *settings) {
	solitaire *s = mem_alloc(context, sizeof(solitaire));

	internal *i = mem_alloc(context, sizeof(internal));
	s->data = i;
	s->visual = visual_create(context, settings);

	i->deal = pile_create(context, 52);
	visual_pile *deal = visual_pile_create(context, i->deal);
	deal->origin[0] = 0 - (5 * (settings->card_width + settings->card_spacing));
	deal->origin[1] = settings->card_height + settings->card_spacing;
	deal->rotation = 30.0f;
	deal->action = action_deal(context, s, i);
	visual_add_pile(context, s->visual, deal);

	i->deck = pile_create(context, 52);
	visual_pile *deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - (3 * (settings->card_width + settings->card_spacing));
	deck->origin[1] = settings->card_height + settings->card_spacing;
	deck->action = action_deck(context, s, i);
	visual_add_pile(context, s->visual, deck);

	i->src = pile_create(context, 52);
	visual_pile *src = visual_pile_create(context, i->src);
	src->origin[0] =
		0 - (1.5 * (settings->card_width + settings->card_spacing));
	src->origin[1] = settings->card_height + settings->card_spacing;
	visual_add_pile(context, s->visual, src);

	visual_pile *ace[4];
	for (int x = 0; x < 4; x++) {
		i->ace[x] = pile_create(context, 13);
		ace[x] = visual_pile_create(context, i->ace[x]),
		ace[x]->origin[0] =
			x * (0 + settings->card_width + settings->card_spacing);
		ace[x]->origin[1] = settings->card_height + settings->card_spacing;
		visual_add_pile(context, s->visual, ace[x]);
	}

	visual_pile *build[7];
	for (int x = 0; x < 7; x++) {
		i->build[x] = pile_create(context, 52);
		build[x] = visual_pile_create(context, i->build[x]);
		build[x]->origin[0] =
			0 - (3 * (settings->card_width + settings->card_spacing)) +
			x * (settings->card_width + settings->card_spacing);
		build[x]->translateY = 0 - settings->card_height / 5;
		visual_add_pile(context, s->visual, build[x]);
	}

	card_create_deck(context, i->deal, 1);
	card_shuffle(i->deal);

	s->ruleset = ruleset_create(context);

	condition *ace1_4_cond = condition_destination_array(
		context, 4, i->ace[0], i->ace[1], i->ace[2], i->ace[3]);
	condition *build1_7_dest_cond = condition_destination_array(
		context, 7, i->build[0], i->build[1], i->build[2], i->build[3],
		i->build[4], i->build[5], i->build[6]);

	/* Allow move of ace to an empty pile among the ace piles. */
	rule *rule1 = rule_create(context);
	rule_add_condition(context, rule1, ace1_4_cond);
	rule_add_condition(context, rule1, condition_destination_empty(context));
	rule_add_condition(context, rule1, condition_top_card(context));
	rule_add_condition(
		context, rule1,
		condition_source_card_equal(context, e_suit_none, 1, e_equal_value, 0));
	rule_add_action(context, rule1, action_reveal_source_top_card(context));
	ruleset_add_rule(context, s->ruleset, rule1);

	/* Allow card to be placed on top of the ace
	   piles following suit and ascending order. */
	rule *rule2 = rule_create(context);
	rule_add_condition(context, rule2, ace1_4_cond);
	rule_add_condition(context, rule2, condition_top_card(context));
	rule_add_condition(context, rule2,
	                   condition_top_card_compare(
						   context, 0, e_dest_1lower_value | e_follow_suit));
	rule_add_action(context, rule2, action_reveal_source_top_card(context));
	ruleset_add_rule(context, s->ruleset, rule2);

	/* Allow moving several cards to a top card of the opposite suit. */
	rule *rule3 = rule_create(context);
	rule_add_condition(context, rule3,
	                   condition_top_card_compare(
						   context, 0, e_dest_1higher_value | e_suit_opposite));
	rule_add_condition(
		context, rule3,
		condition_source_array(context, 8, i->build[0], i->build[1],
	                           i->build[2], i->build[3], i->build[4],
	                           i->build[5], i->build[6], i->src));
	rule_add_condition(context, rule3, build1_7_dest_cond);
	rule_add_condition(context, rule3, condition_source_card_revealed(context));
	rule_add_condition(context, rule3, condition_rest_of_pile(context));
	rule_add_condition(context, rule3,
	                   condition_source_not_equal_destination(context));
	rule_add_action(context, rule3, action_reveal_source_top_card(context));
	ruleset_add_rule(context, s->ruleset, rule3);

	/* Allow moving of kings to an empty pile. */
	rule *rule4 = rule_create(context);
	rule_add_condition(context, rule4, build1_7_dest_cond);
	rule_add_condition(context, rule4, condition_destination_empty(context));
	rule_add_condition(context, rule4,
	                   condition_source_card_equal(context, e_suit_none, 13,
	                                               e_equal_value, 0));
	rule_add_condition(context, rule4, condition_rest_of_pile(context));
	rule_add_action(context, rule4, action_reveal_source_top_card(context));
	ruleset_add_rule(context, s->ruleset, rule4);

	/* Allow moving a single card to the build piles from the ace piles. */
	rule *rule5 = rule_create(context);
	rule_add_condition(context, rule5, condition_top_card(context));
	rule_add_condition(context, rule5,
	                   condition_source_array(context, 4, i->ace[0], i->ace[1],
	                                          i->ace[2], i->ace[3]));
	rule_add_condition(context, rule5, build1_7_dest_cond);
	rule_add_condition(context, rule5,
	                   condition_top_card_compare(
						   context, 0, e_dest_1higher_value | e_suit_opposite));
	ruleset_add_rule(context, s->ruleset, rule5);

	/* Solved rule */
	s->ruleset->solved = rule_create(context);
	rule_add_condition(context, s->ruleset->solved,
	                   condition_card_count_array(context, 13, 4, i->ace[0],
	                                              i->ace[1], i->ace[2],
	                                              i->ace[3]));

	visual_sync(s->visual);

	return s;
}
