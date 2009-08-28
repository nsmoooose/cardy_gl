#include <stdlib.h>
#include <string.h>
#include "rule.h"

/* ------------------------------------------------------------------------- */
typedef struct {
	pile *pile;
} condition_source_data;

static bool condition_source_check(condition *condition, move_action *action) {
	condition_source_data* data = (condition_source_data*)condition->data;
	return data->pile == action->source;
}

condition *condition_source(mem_context *context, pile *pile) {
	condition* c;
	condition_source_data* data;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_source_data));
	data->pile = pile;
	c->data = data;
	c->check = condition_source_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	pile *pile;
} condition_destination_data;

static bool condition_destination_check(condition *condition, move_action *action) {
	condition_destination_data* data = (condition_destination_data*)condition->data;
	return data->pile == action->destination;
}

condition *condition_destination(mem_context *context, pile *pile) {
	condition* c;
	condition_destination_data* data;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_destination_data));
	data->pile = pile;
	c->data = data;
	c->check = condition_destination_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	condition *c1;
	condition *c2;
} condition_or_data;

static bool condition_or_check(condition *cond, move_action *action) {
	condition_or_data *data = (condition_or_data*)cond->data;
	return data->c1->check(data->c1, action) || data->c2->check(data->c2, action);
}

condition *condition_or(mem_context *context, condition *c1, condition *c2) {
	condition *c;
	condition_or_data *data;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_or_data));
	data->c1 = c1;
	data->c2 = c2;
	c->data = data;
	c->check = condition_or_check;
	return c;
}

/* ------------------------------------------------------------------------- */

bool condition_destination_empty_check(condition *cond, move_action *action) {
	return card_count(action->destination) == 0;
}

condition *condition_destination_empty(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_destination_empty_check;
	return c;
}

/* ------------------------------------------------------------------------- */

bool condition_top_card_check(condition *cond, move_action *action) {
	return action->source->cards[action->source_index] == card_last(action->source);
}

condition *condition_top_card(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_top_card_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	pile *dest;
	e_compare_operation operation;
} condition_top_card_compare_data;

bool condition_top_card_compare_check(condition *cond, move_action *action) {
	condition_top_card_compare_data *data = cond->data;
	card *src_card, *dst_card;

	if(card_count(data->dest) == 0) {
		return false;
	}
	src_card = action->source->cards[action->source_index];
	dst_card = card_last(data->dest);
	if(data->operation & e_follow_suit && dst_card->suit != src_card->suit) {
		return false;
	}
	if(data->operation & e_dest_lower_value && dst_card->value >= src_card->value) {
		return false;
	}
	if(data->operation & e_dest_higher_value && dst_card->value <= src_card->value) {
		return false;
	}
	if(data->operation & e_equal_value && dst_card->value != src_card->value) {
		return false;
	}
	return true;
}

condition *condition_top_card_compare(mem_context *context, pile *dest, e_compare_operation operation) {
	condition *c;
	condition_top_card_compare_data *data;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_top_card_compare_data));
	data->dest = dest;
	data->operation = operation;
	c->data = data;
	c->check = condition_top_card_compare_check;
	return c;
}

/* ------------------------------------------------------------------------- */

rule *create_rule(mem_context *context) {
	return mem_alloc(context, sizeof(rule));
}

void rule_add_condition(mem_context *context, rule *rule, condition *cond) {
	condition **old_cond = rule->conditions;

	rule->conditions = mem_alloc(context, (rule->size + 1) * sizeof(condition*));
	rule->size++;

	if(old_cond) {
		memcpy(rule->conditions, old_cond, sizeof(condition*) * (rule->size - 1));
		mem_free(context, old_cond);
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

ruleset *create_ruleset(mem_context *context) {
	return mem_alloc(context, sizeof(ruleset));
}

void ruleset_add_rule(mem_context *context, ruleset *ruleset, rule *new_rule) {
	rule **old_rules = ruleset->rules;

	ruleset->rules = mem_alloc(context, (ruleset->size + 1) * sizeof(rule*));
	ruleset->size++;

	if(old_rules) {
		memcpy(ruleset->rules, old_rules, sizeof(rule*) * (ruleset->size - 1));
		mem_free(context, old_rules);
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

move_action *get_move_action(visual *vis, card_proxy *card, visual_pile *destination_pile) {
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

void apply_move_action(visual *vis, move_action *action) {
	action->destination->cards[action->destination_index] = card_take(action->source, action->source_index);

	/* TODO There is a lot more to handle here. Like:
	 * source_count,
	 * inserting card instead of overwriting.
	 */
}
