#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "rule.h"

/* ------------------------------------------------------------------------- */
typedef struct {
	int card_count;
	pile **piles;
	int size;
} condition_card_count_data;

static bool condition_card_count_array_check(condition *cond,
                                             move_action *action) {
	int index;
	condition_card_count_data *d = cond->data;
	for (index = 0; index < d->size; ++index) {
		if (card_count(d->piles[index]) != d->card_count) {
			return false;
		}
	}
	return true;
}

condition *condition_card_count_array(mem_context *context, int count,
                                      int pile_count, ...) {
	int index;
	va_list vl;
	condition_card_count_data *d =
		mem_alloc(context, sizeof(condition_card_count_data));
	condition *c = mem_alloc(context, sizeof(condition));
	c->data = d;
	c->check = condition_card_count_array_check;

	d->card_count = count;
	d->piles = mem_alloc(context, pile_count * sizeof(pile *));
	d->size = pile_count;
	va_start(vl, pile_count);
	for (index = 0; index < pile_count; ++index) {
		d->piles[index] = va_arg(vl, pile *);
	}
	va_end(vl);
	return c;
}

/* ------------------------------------------------------------------------- */
typedef struct {
	pile **piles;
	int size;
} condition_source_data;

static bool condition_source_check(condition *condition, move_action *action) {
	int index;
	condition_source_data *data = (condition_source_data *)condition->data;
	for (index = 0; index < data->size; ++index) {
		if (data->piles[index] == action->source) {
			return true;
		}
	}
	return false;
}

condition *condition_source(mem_context *context, pile *pile) {
	return condition_source_array(context, 1, pile);
}

condition *condition_source_array(mem_context *context, int count, ...) {
	int index;
	condition *c;
	condition_source_data *data;
	va_list vl;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_source_data));
	data->size = count;
	data->piles = mem_alloc(context, count * sizeof(pile *));
	va_start(vl, count);
	for (index = 0; index < count; ++index) {
		data->piles[index] = va_arg(vl, pile *);
	}
	va_end(vl);
	c->data = data;
	c->check = condition_source_check;
	return c;
}

/* ------------------------------------------------------------------------- */

bool condition_source_card_revealed_check(condition *condition,
                                          move_action *action) {
	if (action->source->cards[action->source_index]->proxy->card == 0) {
		return false;
	}
	return true;
}

condition *condition_source_card_revealed(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_source_card_revealed_check;
	return c;
}

/* ------------------------------------------------------------------------- */

bool condition_source_not_equal_destination_check(condition *cond,
                                                  move_action *action) {
	return action->source != action->destination;
}

condition *condition_source_not_equal_destination(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_source_not_equal_destination_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	pile **piles;
	int size;
} condition_destination_data;

static bool condition_destination_check(condition *condition,
                                        move_action *action) {
	int index;
	condition_destination_data *data =
		(condition_destination_data *)condition->data;
	for (index = 0; index < data->size; ++index) {
		if (data->piles[index] == action->destination) {
			return true;
		}
	}
	return false;
}

condition *condition_destination(mem_context *context, pile *pile) {
	return condition_destination_array(context, 1, pile);
}

condition *condition_destination_array(mem_context *context, int count, ...) {
	int index;
	condition *c;
	condition_destination_data *data;
	va_list vl;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_destination_data));
	data->size = count;
	data->piles = mem_alloc(context, count * sizeof(pile *));
	va_start(vl, count);
	for (index = 0; index < count; ++index) {
		data->piles[index] = va_arg(vl, pile *);
	}
	va_end(vl);
	c->data = data;
	c->check = condition_destination_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	int count;
} condition_move_count_data;

static bool condition_move_count_check(condition *condition,
                                       move_action *action) {
	condition_move_count_data *d = condition->data;
	return action->source_count == d->count;
}

condition *condition_move_count(mem_context *context, int count) {
	condition_move_count_data *d =
		mem_alloc(context, sizeof(condition_move_count_data));
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_move_count_check;
	c->data = d;

	d->count = count;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	condition **conditions;
	int size;
} condition_or_data;

static bool condition_or_check(condition *cond, move_action *action) {
	int index;
	condition_or_data *data = (condition_or_data *)cond->data;
	for (index = 0; index < data->size; ++index) {
		if (data->conditions[index]->check(data->conditions[index], action)) {
			return true;
		}
	}
	return false;
}

condition *condition_or(mem_context *context, condition *c1, condition *c2) {
	return condition_or_array(context, 2, c1, c2);
}

condition *condition_or_array(mem_context *context, int count, ...) {
	int index;
	condition *c;
	condition_or_data *data;
	va_list vl;

	c = mem_alloc(context, sizeof(condition));
	data = mem_alloc(context, sizeof(condition_or_data));
	data->conditions = mem_alloc(context, count * sizeof(condition *));
	data->size = count;
	va_start(vl, count);
	for (index = 0; index < count; ++index) {
		data->conditions[index] = va_arg(vl, condition *);
	}
	va_end(vl);
	c->data = data;
	c->check = condition_or_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	condition *cond;
} condition_not_data;

bool condition_not_check(condition *cond, move_action *action) {
	condition_not_data *data = cond->data;
	return data->cond->check(data->cond, action) == true ? false : true;
}

condition *condition_not(mem_context *context, condition *c) {
	condition *cond = mem_alloc(context, sizeof(condition));
	condition_not_data *data = mem_alloc(context, sizeof(condition_not_data));
	data->cond = c;
	cond->data = data;
	cond->check = condition_not_check;
	return cond;
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

bool condition_rest_of_pile_check(condition *cond, move_action *action) {
	return (card_count(action->source) - action->source_count) ==
		   action->source_index;
}

condition *condition_rest_of_pile(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_rest_of_pile_check;
	return c;
}

/* ------------------------------------------------------------------------- */

bool condition_top_card_check(condition *cond, move_action *action) {
	return action->source->cards[action->source_index] ==
		   card_last(action->source);
}

condition *condition_top_card(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_top_card_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	pile *pile;
	card_suit suit;
	card_value value;
	condition_compare_operation operation;
} condition_card_equal_data;

bool condition_source_card_equal_check(condition *cond, move_action *action) {
	card *card;
	condition_card_equal_data *data = cond->data;

	/* If there has been a pile manually assigned this condition will use
	   the top card. This might be a good idea to send as a parameter as
	   well.
	   Otherwise we will use the card specified with the action. */
	if (data->pile) {
		card = card_last(data->pile);
	} else {
		card = action->source->cards[action->source_index];
	}
	if (card == 0) {
		return false;
	}

	if (data->operation & e_equal_value) {
		if (card->value != data->value) {
			return false;
		}
	}
	if (data->operation & e_follow_suit) {
		if (card->suit != data->suit) {
			return false;
		}
	}
	return true;
}

condition *condition_source_card_equal(mem_context *context, card_suit suit,
                                       card_value value,
                                       condition_compare_operation operation,
                                       pile *pile) {
	condition *c = mem_alloc(context, sizeof(condition));
	condition_card_equal_data *data =
		mem_alloc(context, sizeof(condition_card_equal_data));
	data->pile = pile;
	data->suit = suit;
	data->value = value;
	data->operation = operation;
	c->data = data;
	c->check = condition_source_card_equal_check;
	return c;
}

bool condition_destination_card_equal_check(condition *cond,
                                            move_action *action) {
	card *card;
	condition_card_equal_data *data = cond->data;

	/* If there has been a pile manually assigned this condition will use
	   the top card. This might be a good idea to send as a parameter as
	   well.
	   Otherwise we will use the card specified with the action. */
	if (data->pile) {
		card = card_last(data->pile);
	} else if (action->destination_index <= 0) {
		return false;
	} else {
		card = action->destination->cards[action->destination_index - 1];
	}
	if (card == 0) {
		return false;
	}

	if (data->operation & e_equal_value) {
		if (card->value != data->value) {
			return false;
		}
	}
	if (data->operation & e_follow_suit) {
		if (card->suit != data->suit) {
			return false;
		}
	}
	return true;
}

condition *condition_destination_card_equal(
	mem_context *context, card_suit suit, card_value value,
	condition_compare_operation operation, pile *pile) {
	condition *c = mem_alloc(context, sizeof(condition));
	condition_card_equal_data *data =
		mem_alloc(context, sizeof(condition_card_equal_data));
	data->pile = pile;
	data->suit = suit;
	data->value = value;
	data->operation = operation;
	c->data = data;
	c->check = condition_destination_card_equal_check;
	return c;
}

/* ------------------------------------------------------------------------- */

typedef struct {
	pile *dest;
	condition_compare_operation operation;
} condition_top_card_compare_data;

bool condition_top_card_compare_check(condition *cond, move_action *action) {
	condition_top_card_compare_data *data = cond->data;
	card *src_card, *dst_card;
	pile *dest = data->dest == 0 ? action->destination : data->dest;

	if (card_count(dest) == 0) {
		return false;
	}
	src_card = action->source->cards[action->source_index];
	dst_card = card_last(dest);
	if (data->operation & e_follow_suit && dst_card->suit != src_card->suit) {
		return false;
	}
	if (data->operation & e_dest_lower_value &&
	    dst_card->value >= src_card->value) {
		return false;
	}
	if (data->operation & e_dest_1lower_value &&
	    dst_card->value != src_card->value - 1) {
		return false;
	}
	if (data->operation & e_dest_higher_value &&
	    dst_card->value <= src_card->value) {
		return false;
	}
	if (data->operation & e_dest_1higher_value &&
	    dst_card->value != src_card->value + 1) {
		return false;
	}
	if (data->operation & e_equal_value && dst_card->value != src_card->value) {
		return false;
	}
	if (data->operation & e_suit_opposite) {
		if ((src_card->suit == e_spades || src_card->suit == e_clubs) &&
		    (dst_card->suit == e_spades || dst_card->suit == e_clubs)) {
			return false;
		} else if ((src_card->suit == e_hearts ||
		            src_card->suit == e_diamonds) &&
		           (dst_card->suit == e_hearts ||
		            dst_card->suit == e_diamonds)) {
			return false;
		}
	}
	return true;
}

condition *condition_top_card_compare(mem_context *context, pile *dest,
                                      condition_compare_operation operation) {
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

void action_reveal_source_top_card_execute(rule_action *action,
                                           move_action *move) {
	card *c = card_last(move->source);
	if (c != 0) {
		card_reveal(c);
	}
}

rule_action *action_reveal_source_top_card(mem_context *context) {
	rule_action *a = mem_alloc(context, sizeof(rule_action));
	a->execute = action_reveal_source_top_card_execute;
	return a;
}

/* ------------------------------------------------------------------------- */

rule *rule_create(mem_context *context) {
	return mem_alloc(context, sizeof(rule));
}

void rule_add_condition(mem_context *context, rule *rule, condition *cond) {
	condition **old_cond = rule->conditions;

	rule->conditions =
		mem_alloc(context, (rule->condition_size + 1) * sizeof(condition *));
	rule->condition_size++;

	if (old_cond) {
		memcpy(rule->conditions, old_cond,
		       sizeof(condition *) * (rule->condition_size - 1));
		mem_free(context, old_cond);
	}
	rule->conditions[rule->condition_size - 1] = cond;
}

void rule_add_action(mem_context *context, rule *rule, rule_action *action) {
	rule_action **old = rule->actions;

	rule->actions =
		mem_alloc(context, (rule->action_size + 1) * sizeof(rule_action *));
	rule->action_size++;

	if (old) {
		memcpy(rule->actions, old,
		       sizeof(rule_action *) * (rule->action_size - 1));
		mem_free(context, old);
	}
	rule->actions[rule->action_size - 1] = action;
}

bool rule_check(rule *rule, move_action *action) {
	int index;
	condition *condition;

	for (index = 0; index < rule->condition_size; ++index) {
		condition = rule->conditions[index];
		if (!condition->check(condition, action)) {
			return false;
		}
	}
	return true;
}

void rule_execute_actions(rule *rule, move_action *move) {
	int index;
	for (index = 0; index < rule->action_size; ++index) {
		rule->actions[index]->execute(rule->actions[index], move);
	}
}

ruleset *ruleset_create(mem_context *context) {
	return mem_alloc(context, sizeof(ruleset));
}

void ruleset_add_rule(mem_context *context, ruleset *ruleset, rule *new_rule) {
	rule **old_rules = ruleset->rules;

	ruleset->rules = mem_alloc(context, (ruleset->size + 1) * sizeof(rule *));
	ruleset->size++;

	if (old_rules) {
		memcpy(ruleset->rules, old_rules, sizeof(rule *) * (ruleset->size - 1));
		mem_free(context, old_rules);
	}
	ruleset->rules[ruleset->size - 1] = new_rule;
}

bool ruleset_check(ruleset *ruleset, move_action *action,
                   rule **matching_rule) {
	int index;
	*matching_rule = 0;
	for (index = 0; index < ruleset->size; ++index) {
		if (rule_check(ruleset->rules[index], action)) {
			*matching_rule = ruleset->rules[index];
			return true;
		}
	}
	return false;
}

bool ruleset_move_card(ruleset *ruleset, visual *visual,
                       visual_pile *destination, card_proxy *card, int count) {
	rule *matching_rule;
	bool result;
	move_action *action;

	action = ruleset_get_move_action(visual, card, count, destination);
	result = ruleset_check(ruleset, action, &matching_rule);
	if (result) {
		ruleset_apply_move_action(visual, action);
		rule_execute_actions(matching_rule, action);
	}
	free(action);

	visual_sync(visual);
	return result;
}

bool ruleset_move_individual_card(ruleset *ruleset, visual *visual,
                                  visual_pile *destination, card_proxy *card,
                                  int count) {

	visual_pile *pile = visual_find_pile_from_card(visual, card);
	int card_index = visual_get_card_index(pile, card);

	int i;
	for (i = count - 1 + card_index; i >= (0 + card_index); --i) {
		if (!ruleset_move_card(ruleset, visual, destination, pile->cards[i],
		                       1)) {
			return false;
		}
	}
	return true;
}

move_action *ruleset_get_move_action(visual *vis, card_proxy *card, int count,
                                     visual_pile *destination_pile) {
	int i, j;
	move_action *a = calloc(1, sizeof(move_action));
	for (i = 0; i < vis->pile_count && a->source == 0; ++i) {
		for (j = 0; j < vis->piles[i]->card_count; ++j) {
			if (vis->piles[i]->cards[j] == card) {
				a->source = (pile *)vis->piles[i]->data;
				a->source_index = j;
				break;
			}
		}
	}

	a->source_count = count;
	a->destination = (pile *)destination_pile->data;
	a->destination_index = card_first_free(a->destination);
	return a;
}

void ruleset_apply_move_action(visual *vis, move_action *action) {
	int index;
	card *c;
	for (index = 0; index < action->source_count; ++index) {
		c = card_take(action->source, action->source_index);
		action->destination->cards[card_first_free(action->destination)] = c;
	}

	/* TODO There is a lot more to handle here. Like:
	 * source_count,
	 * inserting card instead of overwriting.
	 */
}
