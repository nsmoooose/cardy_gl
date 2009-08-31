#include <check.h>
#include <stdlib.h>
#include "../lib/rule.h"

bool condition_fail_check(condition *cond, move_action *action) {
	return false;
}
condition *condition_fail(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_fail_check;
	return c;
}

bool condition_succeed_check(condition *cond, move_action *action) {
	return true;
}

condition *condition_succeed(mem_context *context) {
	condition *c = mem_alloc(context, sizeof(condition));
	c->check = condition_succeed_check;
	return c;
}

START_TEST(test_create_rule) {
	mem_context *context = mem_context_create();
	rule *rule = create_rule(context);

	ck_assert_msg(rule != 0, "Failed to create rule.");
	ck_assert_msg(rule->size == 0, "The size of the condition array should initially be 0");
	ck_assert_msg(rule->conditions == 0, "Conditions array should be 0.");
}
END_TEST

START_TEST(test_rule_add_condition) {
	condition *cond;
	rule *rule;
	mem_context *context = mem_context_create();

	cond = condition_source(context, 0);
	rule = create_rule(context);
	rule_add_condition(context, rule, cond);

	ck_assert_msg(rule->size == 1, "Size of rules array should be 1");
	ck_assert_msg(rule->conditions[0] == cond, "First item in array should be condition.");
}
END_TEST

START_TEST(test_rule_check) {
	condition *cond1, *cond2;
	rule *rule;
	move_action action;
	mem_context *context = mem_context_create();

	rule = create_rule(context);
	cond1 = condition_succeed(context);
	rule_add_condition(context, rule, cond1);

	ck_assert_msg(rule_check(rule, &action) == true, "Should have returned true");

	cond2 = condition_fail(context);
	rule_add_condition(context, rule, cond2);

	ck_assert_msg(rule_check(rule, &action) == false, "Should have returned false");
}
END_TEST

START_TEST(test_create_ruleset) {
	ruleset *ruleset;
	mem_context *context = mem_context_create();

	ruleset = create_ruleset(context);

	ck_assert_msg(ruleset != 0, "Failed to create ruleset.");
	ck_assert_msg(ruleset->rules == 0, "rules should initially be 0.");
	ck_assert_msg(ruleset->size == 0, "size should initially be 0.");
}
END_TEST

START_TEST(test_ruleset_add_rule) {
	rule *rule1, *rule2;
	ruleset *ruleset;
	mem_context *context = mem_context_create();

	ruleset = create_ruleset(context);
	rule1 = create_rule(context);
	rule2 = create_rule(context);
	ruleset_add_rule(context, ruleset, rule1);
	ruleset_add_rule(context, ruleset, rule2);

	ck_assert_msg(ruleset->size == 2, "size of ruleset incorrect.");
	ck_assert_msg(ruleset->rules[0] == rule1, "First rule incorrect.");
	ck_assert_msg(ruleset->rules[1] == rule2, "Second rule incorrect.");
}
END_TEST

START_TEST(test_ruleset_check) {
	rule *rule1, *rule2, *matching_rule;
	ruleset *ruleset;
	move_action action;
	mem_context *context = mem_context_create();

	ruleset = create_ruleset(context);
	rule1 = create_rule(context);
	rule_add_condition(context, rule1, condition_fail(context));
	ruleset_add_rule(context, ruleset, rule1);

	ck_assert_msg(ruleset_check(ruleset, &action, &matching_rule) == false, "Check should fail");
	ck_assert_msg(matching_rule == 0, "Matching rule should be set to 0 since no match was made.");

	rule2 = create_rule(context);
	rule_add_condition(context, rule2, condition_succeed(context));
	ruleset_add_rule(context, ruleset, rule2);
	ck_assert_msg(ruleset_check(ruleset, &action, &matching_rule) == true, "Check should succeed");
	ck_assert_msg(matching_rule == rule2, "matching_rule should be equal to rule2");
}
END_TEST

START_TEST(test_condition_source) {
	move_action action;
	pile *p1, *p2;
	condition *cond;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);
	action.source = p1;

	cond = condition_source(context, p1);
	ck_assert_msg(cond->check != 0, "Check method not implemented.");
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");

	action.source = p2;
	ck_assert_msg(cond->check(cond, &action) == false, "Should have returned false");
}
END_TEST

START_TEST(test_condition_or) {
	move_action action;
	condition *cond;
	mem_context *context = mem_context_create();

	cond = condition_or(context, condition_fail(context), condition_fail(context));
	ck_assert_msg(cond->check(cond, &action) == false, "Or operation should be false.");

	cond = condition_or(context, condition_fail(context), condition_succeed(context));
	ck_assert_msg(cond->check(cond, &action) == true, "Or operation should be true.");

	cond = condition_or(context, condition_succeed(context), condition_fail(context));
	ck_assert_msg(cond->check(cond, &action) == true, "Or operation should be true.");
}
END_TEST

START_TEST(test_condition_destination) {
	move_action action;
	pile *p1, *p2;
	condition *cond;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);
	action.destination = p1;

	cond = condition_destination(context, p1);
	ck_assert_msg(cond->check != 0, "Check method not implemented.");
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");

	action.destination = p2;
	ck_assert_msg(cond->check(cond, &action) == false, "Should have returned false");
}
END_TEST

START_TEST(test_condition_destination_empty) {
	pile *p1, *p2;
	condition *cond;
	move_action action;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);
	create_deck(context, p1, 1);

	cond = condition_destination_empty(context);

	action.destination = p1;
	ck_assert_msg(cond->check(cond, &action) == false, "Should return false");

	action.destination = p2;
	ck_assert_msg(cond->check(cond, &action) == true, "Should return true.");
}
END_TEST

START_TEST(test_condition_top_card) {
	pile *deck;
	condition *cond;
	move_action action;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	create_deck(context, deck, 1);
	action.source = deck;
	cond = condition_top_card(context);

	action.source_index = 0;
	ck_assert_msg(cond->check(cond, &action) == false, "Isn't top card.");

	action.source_index = 51;
	ck_assert_msg(cond->check(cond, &action) == true, "Is top card.");
}
END_TEST

START_TEST(test_condition_top_card_equal) {
	move_action action;
	pile *deck;
	condition *cond;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	deck->cards[0] = card_create(context, e_spades, 2);
	action.source = deck;

	cond = condition_top_card_equal(context, e_suit_none, 3, e_equal_value);
	ck_assert_msg(cond->check(cond, &action) == false, "Should fail since card isn't the right value.");

	deck->cards[1] = card_create(context, e_spades, 3);
	ck_assert_msg(cond->check(cond, &action) == true, "Should be movable since top card value is equal.");

	cond = condition_top_card_equal(context, e_clubs, 3, e_equal_value|e_follow_suit);
	ck_assert_msg(cond->check(cond, &action) == false, "Should fail since card suit isn't the right value.");
}
END_TEST

START_TEST(test_condition_top_card_compare) {
	pile *deck, *dest;
	condition *cond;
	move_action action;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	deck->cards[0] = card_create(context, e_spades, 3);
	dest = pile_create(context, 52);

	action.source = deck;
	action.source_index = 0;
	action.destination = dest;

	cond = condition_top_card_compare(context, dest, e_dest_higher_value|e_follow_suit);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination doesn't contain any cards.");

	/* e_follow_suit */
	dest->cards[0] = card_create(context, e_clubs, 2);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains other suit.");

	/* e_dest_higher */
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == true, "true when destination contains higher value.");

	/* e_dest_1higher */
	cond = condition_top_card_compare(context, dest, e_dest_1higher_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == true, "true when destination contains higher value.");

	dest->cards[0] = card_create(context, e_spades, 5);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains higher value more than one step.");

	/* e_dest_lower */
	cond = condition_top_card_compare(context, dest, e_dest_lower_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == true, "true when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains higher value.");

	/* e_dest_1lower */
	cond = condition_top_card_compare(context, dest, e_dest_1lower_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == true, "true when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains higher value.");

	dest->cards[0] = card_create(context, e_spades, 1);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains lower value more than 1 step.");

	/* e_equal_value */
	cond = condition_top_card_compare(context, dest, e_equal_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains higher value.");

	dest->cards[0] = card_create(context, e_spades, 3);
	ck_assert_msg(cond->check(cond, &action) == true, "true when destination contains equal value.");

	/* Use destination from move_action */
	cond = condition_top_card_compare(context, 0, e_equal_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false, "false when destination contains lower value.");
}
END_TEST

START_TEST(test_get_move_action) {
	pile *deck, *done;
	visual *vis;
	move_action *action;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	done = pile_create(context, 52);
	create_deck(context, deck, 1);

	vis = visual_create(context, 0);
	visual_add_pile(context, vis, visual_pile_create(context, deck));
	visual_add_pile(context, vis, visual_pile_create(context, done));
	visual_sync(vis);

	action = get_move_action(vis, vis->piles[0]->cards[4], vis->piles[1]);

	ck_assert_msg(action != 0, "No action returned.");
	ck_assert_msg(action->source == deck, "Deck should be the source.");
	ck_assert_msg(action->source_index == 4, "Source index incorrect.");
	ck_assert_msg(action->source_count == 1, "Source count incorrect.");
	ck_assert_msg(action->destination == done, "Done should be destination.");
	ck_assert_msg(action->destination_index == 0, "Destination index incorrect.");
}
END_TEST

START_TEST(test_apply_move_action) {
	pile *deck, *done;
	visual *vis;
	move_action *action;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	done = pile_create(context, 52);
	create_deck(context, deck, 1);

	vis = visual_create(context, 0);
	visual_add_pile(context, vis, visual_pile_create(context, deck));
	visual_add_pile(context, vis, visual_pile_create(context, done));
	visual_sync(vis);

	action = get_move_action(vis, vis->piles[0]->cards[51], vis->piles[1]);
	apply_move_action(vis, action);
	visual_sync(vis);

	ck_assert_msg(vis->piles[0]->card_count == 51, "Card count should be one less.");
	ck_assert_msg(vis->piles[1]->card_count == 1, "Card count should be one more.");
	ck_assert_msg(vis->piles[0]->cards[51] == 0, "Card should have been moved.");
	ck_assert_msg(vis->piles[1]->cards[0] != 0, "Card should have been moved here.");
}
END_TEST

void add_rule_tests(Suite *suite) {
	TCase *rule_case = tcase_create("Rules");
	tcase_add_test(rule_case, test_condition_source);
	tcase_add_test(rule_case, test_condition_or);
	tcase_add_test(rule_case, test_condition_destination);
	tcase_add_test(rule_case, test_condition_destination_empty);
	tcase_add_test(rule_case, test_condition_top_card);
	tcase_add_test(rule_case, test_condition_top_card_equal);
	tcase_add_test(rule_case, test_condition_top_card_compare);
	tcase_add_test(rule_case, test_create_rule);
	tcase_add_test(rule_case, test_rule_add_condition);
	tcase_add_test(rule_case, test_rule_check);
	tcase_add_test(rule_case, test_create_ruleset);
	tcase_add_test(rule_case, test_ruleset_add_rule);
	tcase_add_test(rule_case, test_ruleset_check);
	tcase_add_test(rule_case, test_get_move_action);
	tcase_add_test(rule_case, test_apply_move_action);
	suite_add_tcase(suite, rule_case);
}
