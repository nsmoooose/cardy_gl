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

typedef struct condition_St {
	void *data;
	bool (*check)(struct condition_St* condition, move_action *action);
} condition;

typedef struct {
	condition **conditions;
	int size;
} rule;

typedef struct {
	rule **rules;
	int size;
} ruleset;

typedef enum {
	e_equal_value=1,
	e_dest_lower_value=2,
	e_dest_higher_value=4,
	e_dest_1lower_value=8,
	e_dest_1higher_value=16,
	e_follow_suit=32
} e_compare_operation;

condition *condition_or(mem_context *context, condition *c1, condition *c2);
condition *condition_source(mem_context *context, pile *pile);
condition *condition_top_card(mem_context *context);
condition *condition_top_card_compare(mem_context *context, pile *dest, e_compare_operation operation);
condition *condition_top_card_equal(mem_context *context, card_suit suit, card_value value, e_compare_operation operation);
condition *condition_destination(mem_context *context, pile *pile);
condition *condition_destination_empty(mem_context *context);

rule *create_rule(mem_context *context);
void rule_add_condition(mem_context *context, rule *rule, condition *condition);
bool rule_check(rule *rule, move_action *action);

ruleset *create_ruleset(mem_context *context);
void ruleset_add_rule(mem_context *context, ruleset *ruleset, rule *rule);
bool ruleset_check(ruleset *ruleset, move_action *action, rule **matching_rule);

move_action *get_move_action(visual *vis, card_proxy *card, visual_pile *destination_pile);
void apply_move_action(visual *vis, move_action *action);

#endif /* __RULE_H__ */
