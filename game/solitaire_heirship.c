#include "solitaire_heirship.h"

typedef struct {
	pile *deck;
	pile *kings[8];
	pile *queens[8];
	pile *aces[8];
} internal;

typedef struct {
	solitaire *sol;
	internal *i;
} action_deal_data;

static void action_deal_execute(visual_pile_action *action) {
	action_deal_data *data = action->data;
	internal *i = data->i;
	solitaire *sol = data->sol;

	if(card_count(i->deck) == 104) {
		int ki = 0, qi = 0, ai = 0;
		card_reveal_all(i->deck);
		while(card_count(i->deck) > 0) {
			card *card = card_take_last(i->deck);
			if(card->value == 1) {
				card_append(card, i->aces[ai]);
				ai++;
			}
			else if(card->value == 12) {
				card_append(card, i->queens[qi]);
				qi++;
			}
			else if(card->value == 13) {
				card_append(card, i->kings[ki]);
				ki++;
			}
			else {
				card_append(card, i->kings[ki]);
			}
		}
	}
	else {
		card_move_all_array(
			i->deck, 24,
			i->kings[0], i->kings[1], i->kings[2], i->kings[3],
			i->kings[4], i->kings[5], i->kings[6], i->kings[7],

			i->queens[0], i->queens[1], i->queens[2], i->queens[3],
			i->queens[4], i->queens[5], i->queens[6], i->queens[7],

			i->aces[0], i->aces[1], i->aces[2], i->aces[3],
			i->aces[4], i->aces[5], i->aces[6], i->aces[7]);
		card_hide_all(i->deck);
		card_shuffle(i->deck);
	}

	visual_sync(sol->visual);
}

static visual_pile_action *action_deal(
	mem_context *context, solitaire *sol, internal *i) {
	visual_pile_action *deal_action = mem_alloc(context, sizeof(visual_pile_action));
	action_deal_data *data = mem_alloc(context, sizeof(action_deal_data));
	data->sol = sol;
	data->i = i;
	deal_action->data = data;
	deal_action->execute = action_deal_execute;
	return deal_action;
}

static void setup_rules(mem_context *context, solitaire *s, internal *i) {
	rule *rule1;

	s->ruleset = ruleset_create(context);
	rule1 = rule_create(context);
	rule_add_condition(context, rule1, condition_destination_empty(context));
	ruleset_add_rule(context, s->ruleset, rule1);

	/*
	s->ruleset->solved = rule_create(context);
	rule_add_condition(
		context, s->ruleset->solved,
		condition_card_count_array(context, 1, 1, i->done));
	*/
}

solitaire* solitaire_heirship(mem_context *context, visual_settings *settings) {
	int index;
	float left, left_tmp;
	visual_pile *deck, *vpile;
	internal *i = mem_alloc(context, sizeof(internal));
	solitaire *s = mem_alloc(context, sizeof(solitaire));
	s->data = i;
	s->visual = visual_create(context, settings);

	i->deck = pile_create(context, 104);
	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - settings->card_width * 6 - settings->card_spacing * 6;
	deck->rotation = 45;
	deck->action = action_deal(context, s, i);
	visual_add_pile(context, s->visual, deck);

	left = 0 - settings->card_spacing / 2 -
		settings->card_width * 4 -
		settings->card_spacing * 4;
	for(index=0;index<8;++index) {
		left_tmp = left + (settings->card_width +
								   settings->card_spacing) * index;

		i->kings[index] = pile_create(context, 104);
		vpile = visual_pile_create(context, i->kings[index]);
		vpile->origin[0] = left_tmp;
		vpile->origin[1] = settings->card_height + settings->card_spacing;
		visual_add_pile(context, s->visual, vpile);

		i->queens[index] = pile_create(context, 1);
		vpile = visual_pile_create(context, i->queens[index]);
		vpile->origin[0] = left_tmp;
		visual_add_pile(context, s->visual, vpile);

		i->aces[index] = pile_create(context, 52);
		vpile = visual_pile_create(context, i->aces[index]);
		vpile->origin[0] = left_tmp;
		vpile->origin[1] = 0 - settings->card_height - settings->card_spacing;
		visual_add_pile(context, s->visual, vpile);
	}

	create_deck(context, i->deck, 1);
	create_deck(context, i->deck, 1);
	card_shuffle(i->deck);

	visual_sync(s->visual);

	setup_rules(context, s, i);

	return s;
}
