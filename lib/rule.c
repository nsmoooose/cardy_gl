#include <stdlib.h>
#include <string.h>
#include "rule.h"

typedef struct {
	pile *pile;
} condition_source_data;

static bool condition_source_check(condition *condition, move_action *action) {
	condition_source_data* data = (condition_source_data*)condition->data;
	return data->pile == action->source;
}

condition *condition_source(pile *pile) {
	condition* c;
	condition_source_data* data;

	c = calloc(1, sizeof(condition));
	data = calloc(1, sizeof(condition_source_data));
	data->pile = pile;
	c->data = data;
	c->check = condition_source_check;
	return c;
}

rule *create_rule() {
	return calloc(1, sizeof(rule));
}

void rule_add_condition(rule *rule, condition *cond) {
	condition **old_cond = rule->conditions;

	rule->conditions = calloc(rule->size + 1, sizeof(condition*));
	rule->size++;

	if(old_cond) {
		memcpy(rule->conditions, old_cond, sizeof(condition*) * (rule->size - 1));
		free(old_cond);
	}
	rule->conditions[rule->size - 1] = cond;
}

bool rule_check(rule *rule, move_action *action) {
	int index;
	condition *condition;

	for(index=0;index<rule->size;++index) {
		condition = rule->conditions[index];
		if(!condition->check(condition, action)) {
			return false;
		}
	}
	return true;
}

#if 0
void stub() {
	pile *pile1, *pile2, *pile3, *pile4, *done;
	rulebook *rulebook;
	rule *rule1, *rule2;

	ruleset = create_ruleset();

	/* Move card to done pile if source is pile1-pile4 and there is
	   a higher card in same suit in those piles. */
	rule1 = create_rule();
	rule_add_condition(
		rule1,
		condition_or(
			condition_or(
				condition_or(
					condition_source(pile1),
					condition_source(pile2)),
				condition_source(pile3)),
			condition_source(pile4))
		);
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
	rule_add_condition(
		rule2,
		condition_or(
			condition_or(
				condition_or(
					condition_source(pile1),
					condition_source(pile2)),
				condition_source(pile3)),
			condition_source(pile4))
		);
	rule_add_condition(rule1, condition_top_card());
	rule_add_condition(rule2, condition_destination_empty());
	ruleset_add_rule(ruleset, rule2);

	ruleset_check(ruleset, action);
}
#endif
