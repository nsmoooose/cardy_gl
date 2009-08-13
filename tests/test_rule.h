#include <check.h>
#include "../lib/rule.h"

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
	pile *pile1, *pile2;
	condition *cond1, *cond2;
	rule *rule;
	move_action action;

	pile1 = pile_create(12);
	pile2 = pile_create(12);

	rule = create_rule();
	cond1 = condition_source(pile1);
	rule_add_condition(rule, cond1);

	action.source = pile1;

	ck_assert_msg(rule_check(rule, &action) == true, "Should have returned true");

	cond2 = condition_source(pile2);
	rule_add_condition(rule, cond2);

	ck_assert_msg(rule_check(rule, &action) == false, "Should have returned false");
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
