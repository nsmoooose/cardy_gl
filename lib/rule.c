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

ruleset *create_ruleset() {
	return calloc(1, sizeof(ruleset));
}

void ruleset_add_rule(ruleset *ruleset, rule *new_rule) {
	rule **old_rules = ruleset->rules;

	ruleset->rules = calloc(ruleset->size + 1, sizeof(rule*));
	ruleset->size++;

	if(old_rules) {
		memcpy(ruleset->rules, old_rules, sizeof(rule*) * (ruleset->size - 1));
		free(old_rules);
	}
	ruleset->rules[ruleset->size - 1] = new_rule;
}

bool ruleset_check(ruleset *ruleset, move_action *action) {
	int index;
	for(index = 0;index<ruleset->size;++index) {
		if(rule_check(ruleset->rules[index], action)) {
			return true;
		}
	}
	return false;
}

move_action *get_move_action(visual *vis, card_proxy *card, vis_pile *destination_pile) {
	int i, j;
	move_action *a = calloc(1, sizeof(move_action));
	for(i=0;i<vis->pile_count && a->source == 0;++i) {
		for(j=0;j<vis->piles[i]->card_count;++j) {
			if(vis->piles[i]->cards[j] == card) {
				a->source = (pile*)vis->piles[i]->data;
				a->source_index = j;
				break;
			}
		}
	}

	a->source_count = 1;
	a->destination = (pile*)destination_pile->data;
	a->destination_index = card_first_free(a->destination);
	return a;
}
