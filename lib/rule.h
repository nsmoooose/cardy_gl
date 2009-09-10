#ifndef __RULE_H__
#define __RULE_H__

#include "card.h"

typedef struct {
	pile *source;
	int source_index;
	int source_count;

	pile *destination;
	int destination_index;
} move_action;

typedef struct rule_action_St {
	void *data;
	void (*execute)(struct rule_action_St* action, move_action *move);
} rule_action;

typedef struct condition_St {
	void *data;
	bool (*check)(struct condition_St* condition, move_action *action);
} condition;

typedef struct {
	condition **conditions;
	int condition_size;

	rule_action **actions;
	int action_size;
} rule;

typedef struct {
	rule **rules;
	int size;

	rule *solved;
} ruleset;

typedef enum {
	e_equal_value=1,
	e_dest_lower_value=2,
	e_dest_higher_value=4,
	e_dest_1lower_value=8,
	e_dest_1higher_value=16,
	e_follow_suit=32,

	/** Used in condition when an opposite suit is used. Opposite
	 *  uses the color as comparison. red is opposite to black.
	 */
	e_suit_opposite=64
} condition_compare_operation;

condition *condition_or(mem_context *context, condition *c1, condition *c2);
condition *condition_or_array(mem_context *context, int count, ...);
condition *condition_source(mem_context *context, pile *pile);
condition *condition_source_array(mem_context *context, int count, ...);
condition *condition_top_card(mem_context *context);
condition *condition_top_card_compare(
	mem_context *context, pile *dest, condition_compare_operation operation);
condition *condition_top_card_equal(
	mem_context *context, card_suit suit, card_value value,
	condition_compare_operation operation, pile *pile);
condition *condition_destination(mem_context *context, pile *pile);
condition *condition_destination_array(mem_context *context, int count, ...);
condition *condition_destination_empty(mem_context *context);

rule_action *action_reveal_source_top_card(mem_context *context);

rule *rule_create(mem_context *context);
void rule_add_condition(mem_context *context, rule *rule, condition *condition);
void rule_add_action(mem_context *context, rule *rule, rule_action *action);
bool rule_check(rule *rule, move_action *action);
void rule_execute_actions(rule *rule, move_action *move);

ruleset *ruleset_create(mem_context *context);
void ruleset_add_rule(mem_context *context, ruleset *ruleset, rule *rule);
bool ruleset_check(ruleset *ruleset, move_action *action, rule **matching_rule);
move_action *ruleset_get_move_action(
	visual *vis, card_proxy *card, int count, visual_pile *destination_pile);
void ruleset_apply_move_action(visual *vis, move_action *action);
bool ruleset_move_card(
	ruleset *ruleset, visual *visual, visual_pile *destination,
	card_proxy *card, int count);

#endif /* __RULE_H__ */
