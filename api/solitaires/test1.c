#include "test1.h"

typedef struct {
	pile *deck;
	pile *done;
} internal;

static void setup_rules(mem_context *context, solitaire *s, internal *i) {
	rule *rule1;

	s->ruleset = ruleset_create(context);
	rule1 = rule_create(context);
	rule_add_condition(context, rule1, condition_destination_empty(context));
	ruleset_add_rule(context, s->ruleset, rule1);

	s->ruleset->solved = rule_create(context);
	rule_add_condition(context, s->ruleset->solved,
	                   condition_card_count_array(context, 1, 1, i->done));
}

solitaire *solitaire_test1(mem_context *context, visual_settings *settings) {
	visual_pile *deck, *done;
	internal *i = mem_alloc(context, sizeof(internal));
	solitaire *s = mem_alloc(context, sizeof(solitaire));
	s->data = i;
	s->visual = visual_create(context, settings);

	i->deck = pile_create(context, 52);
	deck = visual_pile_create(context, i->deck);
	deck->origin[0] = 0 - settings->card_width - settings->card_spacing / 2;
	visual_add_pile(context, s->visual, deck);

	i->done = pile_create(context, 52);
	done = visual_pile_create(context, i->done);
	done->origin[0] = settings->card_width - settings->card_spacing / 2;
	visual_add_pile(context, s->visual, done);

	card_create_deck(context, i->deck, 1);
	visual_sync(s->visual);

	setup_rules(context, s, i);

	return s;
}
