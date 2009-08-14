#include <check.h>
#include "../lib/rule.h"

bool condition_fail_check(condition *cond, move_action *action) {
	return false;
}
condition *condition_fail() {
	condition *c = calloc(1, sizeof(condition));
	c->check = condition_fail_check;
	return c;
}

bool condition_succeed_check(condition *cond, move_action *action) {
	return true;
}

condition *condition_succeed() {
	condition *c = calloc(1, sizeof(condition));
	c->check = condition_succeed_check;
	return c;
}

START_TEST(test_create_rule) {
	rule *rule = create_rule();

	ck_assert_msg(rule != 0, "Failed to create rule.");
	ck_assert_msg(rule->size == 0, "The size of the condition array should initially be 0");
	ck_assert_msg(rule->conditions == 0, "Conditions array should be 0.");
}
END_TEST

START_TEST(test_rule_add_condition) {
	condition *cond;
	rule *rule;

	cond = condition_source(0);
	rule = create_rule();
	rule_add_condition(rule, cond);

	ck_assert_msg(rule->size == 1, "Size of rules array should be 1");
	ck_assert_msg(rule->conditions[0] == cond, "First item in array should be condition.");
}
END_TEST

START_TEST(test_rule_check) {
	condition *cond1, *cond2;
	rule *rule;
	move_action action;

	rule = create_rule();
	cond1 = condition_succeed();
	rule_add_condition(rule, cond1);

	ck_assert_msg(rule_check(rule, &action) == true, "Should have returned true");

	cond2 = condition_fail();
	rule_add_condition(rule, cond2);

	ck_assert_msg(rule_check(rule, &action) == false, "Should have returned false");
}
END_TEST

START_TEST(test_create_ruleset) {
	ruleset *ruleset;

	ruleset = create_ruleset();

	ck_assert_msg(ruleset != 0, "Failed to create ruleset.");
	ck_assert_msg(ruleset->rules == 0, "rules should initially be 0.");
	ck_assert_msg(ruleset->size == 0, "size should initially be 0.");
}
END_TEST

START_TEST(test_ruleset_add_rule) {
	rule *rule1, *rule2;
	ruleset *ruleset;

	ruleset = create_ruleset();
	rule1 = create_rule();
	rule2 = create_rule();
	ruleset_add_rule(ruleset, rule1);
	ruleset_add_rule(ruleset, rule2);

	ck_assert_msg(ruleset->size == 2, "size of ruleset incorrect.");
	ck_assert_msg(ruleset->rules[0] == rule1, "First rule incorrect.");
	ck_assert_msg(ruleset->rules[1] == rule2, "Second rule incorrect.");
}
END_TEST

START_TEST(test_ruleset_check) {
	rule *rule1, *rule2;
	ruleset *ruleset;
	move_action action;

	ruleset = create_ruleset();
	rule1 = create_rule();
	rule_add_condition(rule1, condition_fail());
	ruleset_add_rule(ruleset, rule1);

	ck_assert_msg(ruleset_check(ruleset, &action) == false, "Check should fail");

	rule2 = create_rule();
	rule_add_condition(rule2, condition_succeed());
	ruleset_add_rule(ruleset, rule2);
	ck_assert_msg(ruleset_check(ruleset, &action) == true, "Check should succeed");
}
END_TEST

START_TEST(test_condition_source) {
	move_action action;
	pile *p1, *p2;
	condition *cond;

	p1 = pile_create(52);
	p2 = pile_create(52);
	action.source = p1;

	cond = condition_source(p1);
	ck_assert_msg(cond->check != 0, "Check method not implemented.");
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");

	action.source = p2;
	ck_assert_msg(cond->check(cond, &action) == false, "Should have returned false");
}
END_TEST

START_TEST(test_condition_or) {
	move_action action;
	condition *cond;

	cond = condition_or(condition_fail(), condition_fail());
	ck_assert_msg(cond->check(cond, &action) == false, "Or operation should be false.");

	cond = condition_or(condition_fail(), condition_succeed());
	ck_assert_msg(cond->check(cond, &action) == true, "Or operation should be true.");

	cond = condition_or(condition_succeed(), condition_fail());
	ck_assert_msg(cond->check(cond, &action) == true, "Or operation should be true.");
}
END_TEST

START_TEST(test_condition_destination_empty) {
	pile *p1, *p2;
	condition *cond;
	move_action action;

	p1 = pile_create(52);
	p2 = pile_create(52);
	create_deck(p1);

	cond = condition_destination_empty();

	action.destination = p1;
	ck_assert_msg(cond->check(cond, &action) == false, "Should return false");

	action.destination = p2;
	ck_assert_msg(cond->check(cond, &action) == true, "Should return true.");
}
END_TEST
