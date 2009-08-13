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

condition *condition_or(condition *c1, condition *c2);
condition *condition_source(pile *pile);
condition *condition_top_card();
condition *condition_destination(pile *pile);

rule *create_rule();
void rule_add_condition(rule *rule, condition *condition);
bool rule_check(rule *rule, move_action *action);
void rule_free(rule *rule);

ruleset *create_ruleset();
void ruleset_add_rule(ruleset *ruleset, rule *rule);
bool ruleset_check(ruleset *ruleset, move_action *action);
void ruleset_free(ruleset *ruleset);

#endif /* __RULE_H__ */
