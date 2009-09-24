#include <check.h>
#include <stdlib.h>
#include "../api/rule.h"

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

START_TEST(test_rule_create) {
	mem_context *context = mem_context_create();
	rule *rule = rule_create(context);

	ck_assert_msg(rule != 0, "Failed to create rule.");
	ck_assert_msg(rule->condition_size == 0,
				  "The size of the condition array should initially be 0");
	ck_assert_msg(rule->conditions == 0, "Conditions array should be 0.");
	ck_assert_msg(rule->action_size == 0,
				  "The size of the action array should initially be 0");
	ck_assert_msg(rule->actions == 0, "Action array should be 0.");
}
END_TEST

START_TEST(test_rule_add_condition) {
	condition *cond;
	rule *rule;
	mem_context *context = mem_context_create();

	cond = condition_source(context, 0);
	rule = rule_create(context);
	rule_add_condition(context, rule, cond);

	ck_assert_msg(rule->condition_size == 1, "Size of rules array should be 1");
	ck_assert_msg(rule->conditions[0] == cond,
				  "First item in array should be condition.");
}
END_TEST

START_TEST(test_rule_add_action) {
	rule_action *action1, *action2;
	rule *rule;
	mem_context *context = mem_context_create();

	rule = rule_create(context);
	action1 = action_reveal_source_top_card(context);
	action2 = action_reveal_source_top_card(context);
	rule_add_action(context, rule, action1);
	rule_add_action(context, rule, action2);

	ck_assert_msg(rule->action_size == 2, "Array should now be 2.");
	ck_assert_msg(rule->actions[0] == action1,
				  "First element should be the first action.");
	ck_assert_msg(rule->actions[1] == action2,
				  "Second element should be the second action.");
}
END_TEST

START_TEST(test_rule_check) {
	condition *cond1, *cond2;
	rule *rule;
	move_action action;
	mem_context *context = mem_context_create();

	rule = rule_create(context);
	cond1 = condition_succeed(context);
	rule_add_condition(context, rule, cond1);

	ck_assert_msg(rule_check(rule, &action) == true,
				  "Should have returned true");

	cond2 = condition_fail(context);
	rule_add_condition(context, rule, cond2);

	ck_assert_msg(rule_check(rule, &action) == false,
				  "Should have returned false");
}
END_TEST

START_TEST(test_rule_execute_actions) {
	pile *deck;
	rule *rule;
	move_action move;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	create_deck(context, deck, 1);
	move.source = deck;
	rule = rule_create(context);
	rule_add_action(context, rule, action_reveal_source_top_card(context));

	rule_execute_actions(rule, &move);

	ck_assert_msg(deck->cards[51]->proxy->card != 0,
				  "Card should have been revealed if action had been executed.");
}
END_TEST

START_TEST(test_ruleset_create) {
	ruleset *ruleset;
	mem_context *context = mem_context_create();

	ruleset = ruleset_create(context);

	ck_assert_msg(ruleset != 0, "Failed to create ruleset.");
	ck_assert_msg(ruleset->rules == 0, "rules should initially be 0.");
	ck_assert_msg(ruleset->size == 0, "size should initially be 0.");
}
END_TEST

START_TEST(test_ruleset_add_rule) {
	rule *rule1, *rule2;
	ruleset *ruleset;
	mem_context *context = mem_context_create();

	ruleset = ruleset_create(context);
	rule1 = rule_create(context);
	rule2 = rule_create(context);
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

	ruleset = ruleset_create(context);
	rule1 = rule_create(context);
	rule_add_condition(context, rule1, condition_fail(context));
	ruleset_add_rule(context, ruleset, rule1);

	ck_assert_msg(
		ruleset_check(ruleset, &action, &matching_rule) == false,
		"Check should fail");
	ck_assert_msg(matching_rule == 0, "Matching rule should be set to 0 "
				  "since no match was made.");

	rule2 = rule_create(context);
	rule_add_condition(context, rule2, condition_succeed(context));
	ruleset_add_rule(context, ruleset, rule2);
	ck_assert_msg(
		ruleset_check(ruleset, &action, &matching_rule) == true,
		"Check should succeed");
	ck_assert_msg(matching_rule == rule2, "matching_rule should be equal to rule2");
}
END_TEST

START_TEST(test_ruleset_move_card) {
	mem_context *context = mem_context_create();
	visual *vis;
	pile *deck, *done;
	visual_pile *vis_deck, *vis_done;
	ruleset *ruleset;
	rule *rule1;
	bool result;

	/* Arrange */
	vis = visual_create(context, 0);

	deck = pile_create(context, 52);
	create_deck(context, deck, 1);
	vis_deck = visual_pile_create(context, deck);
	visual_add_pile(context, vis, vis_deck);

	done = pile_create(context, 52);
	vis_done = visual_pile_create(context, done);
	visual_add_pile(context, vis, vis_done);
	visual_sync(vis);

	ruleset = ruleset_create(context);
	rule1 = rule_create(context);
	rule_add_condition(context, rule1, condition_succeed(context));
	rule_add_action(context, rule1, action_reveal_source_top_card(context));
	ruleset_add_rule(context, ruleset, rule1);

	/* Act and assert */
	result = ruleset_move_card(ruleset, vis, vis_done, vis->piles[0]->cards[51], 1);
	ck_assert_msg(result == true, "Move should have been allowed.");
	ck_assert_msg(vis->piles[0]->cards[51] == 0, "Card wasn't moved.");
	ck_assert_msg(vis->piles[1]->cards[0] != 0, "Card wasn't moved.");
	ck_assert_msg(vis->piles[0]->cards[50]->card != 0, "Card wasn't revealed.");

	/* Act and assert */
	rule_add_condition(context, rule1, condition_fail(context));
	result = ruleset_move_card(ruleset, vis, vis_done, vis->piles[0]->cards[50], 1);
	ck_assert_msg(result == false, "Move should not have been allowed.");
	ck_assert_msg(vis->piles[0]->cards[50] != 0, "Card was moved.");
	ck_assert_msg(vis->piles[1]->cards[1] == 0, "Card was moved.");
}
END_TEST

START_TEST(test_ruleset_move_individual_card) {
	mem_context *context = mem_context_create();
	visual *vis;
	pile *deck, *done;
	ruleset *ruleset;
	rule *rule1;

	/* Arrange */
	vis = visual_create(context, 0);

	deck = pile_create(context, 52);
	deck->cards[0] = card_create(context, e_spades, 1);
	deck->cards[1] = card_create(context, e_spades, 2);
	visual_add_pile(context, vis, visual_pile_create(context, deck));

	done = pile_create(context, 52);
	visual_add_pile(context, vis, visual_pile_create(context, done));
	visual_sync(vis);

	ruleset = ruleset_create(context);
	rule1 = rule_create(context);
	rule_add_condition(context, rule1, condition_succeed(context));
	ruleset_add_rule(context, ruleset, rule1);

	ck_assert(
		ruleset_move_individual_card(
			ruleset, vis, vis->piles[1], vis->piles[0]->cards[0], 2) == true);
	ck_assert(card_count(done) == 2);
	ck_assert(done->cards[0]->value == 2);
	ck_assert(done->cards[1]->value == 1);
}
END_TEST

START_TEST(test_condition_card_count_array) {
	pile *p1, *p2;
	condition *cond;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);

	cond = condition_card_count_array(context, 52, 2, p1, p2);

	create_deck(context, p1, 1);
	ck_assert_msg(cond->check(cond, 0) == false, "Should be false.");

	create_deck(context, p2, 1);
	ck_assert_msg(cond->check(cond, 0) == true, "Should be true.");
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

START_TEST(test_condition_source_array) {
	move_action action;
	pile *p1, *p2, *p3;
	condition *cond;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);
	p3 = pile_create(context, 52);

	cond = condition_source_array(context, 2, p1, p2);
	ck_assert_msg(cond->check != 0, "Check method not implemented.");

	action.source = p1;
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");
	action.source = p2;
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");

	action.source = p3;
	ck_assert_msg(cond->check(cond, &action) == false, "Should have returned false");
}
END_TEST

START_TEST(test_condition_source_card_revealed) {
	move_action action;
	pile *p1;
	condition *cond;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p1->cards[0] = card_create(context, e_clubs, 1);

	cond = condition_source_card_revealed(context);
	ck_assert_msg(cond->check != 0, "Check method not implemented.");

	action.source = p1;
	action.source_index = 0;
	ck_assert_msg(cond->check(cond, &action) == false, "Should have returned "
				  "false when card hasn't been revealed yet.");

	card_reveal(p1->cards[0]);
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned "
				  "false when card hasn't been revealed yet.");
}
END_TEST

START_TEST(test_condition_source_not_equal_destination) {
	move_action action;
	condition *cond;
	mem_context *context = mem_context_create();
	pile *p1, *p2;

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);
	cond = condition_source_not_equal_destination(context);

	action.source = p1;
	action.destination = p2;
	ck_assert(cond->check(cond, &action) == true);

	action.destination = p1;
	ck_assert(cond->check(cond, &action) == false);
}
END_TEST

START_TEST(test_condition_move_count) {
	move_action action;
	condition *cond;
	mem_context *context = mem_context_create();
	cond = condition_move_count(context, 3);

	action.source_count = 3;
	ck_assert(cond->check(cond, &action) == true);

	action.source_count = 4;
	ck_assert(cond->check(cond, &action) == false);
}
END_TEST

START_TEST(test_condition_or) {
	move_action action;
	condition *cond;
	mem_context *context = mem_context_create();

	cond = condition_or(context, condition_fail(context), condition_fail(context));
	ck_assert_msg(cond->check(cond, &action) == false,
				  "Or operation should be false.");

	cond = condition_or(context, condition_fail(context),
						condition_succeed(context));
	ck_assert_msg(cond->check(cond, &action) == true,
				  "Or operation should be true.");

	cond = condition_or(context, condition_succeed(context),
						condition_fail(context));
	ck_assert_msg(cond->check(cond, &action) == true,
				  "Or operation should be true.");
}
END_TEST

START_TEST(test_condition_or_array) {
	mem_context *context = mem_context_create();
	condition *cond, *or1, *or2, *or3, *or4;

	or1 = condition_fail(context);
	or2 = condition_fail(context);
	or3 = condition_fail(context);
	or4 = condition_succeed(context);

	cond = condition_or_array(context, 4, or1, or2, or3, or4);
	ck_assert_msg(cond->check(cond, 0) == true, "Should have returned true.");
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
	ck_assert_msg(cond->check(cond, &action) == false,
				  "Should have returned false");
}
END_TEST

START_TEST(test_condition_destination_array) {
	move_action action;
	pile *p1, *p2, *p3;
	condition *cond;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	p2 = pile_create(context, 52);
	p3 = pile_create(context, 52);

	cond = condition_destination_array(context, 2, p1, p2);
	ck_assert_msg(cond->check != 0, "Check method not implemented.");

	action.destination = p1;
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");
	action.destination = p2;
	ck_assert_msg(cond->check(cond, &action) == true, "Should have returned true");

	action.destination = p3;
	ck_assert_msg(cond->check(cond, &action) == false,
				  "Should have returned false");
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

START_TEST(test_condition_rest_of_pile) {
	pile *p1;
	condition *cond;
	move_action action;
	mem_context *context = mem_context_create();

	p1 = pile_create(context, 52);
	create_deck(context, p1, 1);

	cond = condition_rest_of_pile(context);

	action.source = p1;
	action.source_index = 1;
	action.source_count = 51;
	ck_assert_msg(cond->check(cond, &action) == true, "Should return true.");

	action.source_index = 0;
	ck_assert_msg(cond->check(cond, &action) == false, "Should return false.");
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

START_TEST(test_condition_card_equal) {
	move_action action;
	pile *deck;
	condition *cond;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	deck->cards[0] = card_create(context, e_spades, 2);
	action.source = deck;
	action.source_index = 0;

	cond = condition_card_equal(context, e_suit_none, 3, e_equal_value, 0);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "Should fail since card isn't the right value.");

	action.source_index = 1;
	deck->cards[1] = card_create(context, e_spades, 3);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "Should be movable since top card value is equal.");

	cond = condition_card_equal(
		context, e_clubs, 3, e_equal_value|e_follow_suit, 0);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "Should fail since card suit isn't the right value.");

	cond = condition_card_equal(
		context, e_clubs, 2, e_equal_value|e_follow_suit, deck);
	ck_assert_msg(cond->check(cond, 0) == false,
				  "Should succeed since card is the right value.");
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

	cond = condition_top_card_compare(context, dest,
									  e_dest_higher_value|e_follow_suit);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination doesn't contain any cards.");

	/* e_follow_suit */
	dest->cards[0] = card_create(context, e_clubs, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains other suit.");

	/* e_suit_opposite */
	cond = condition_top_card_compare(context, dest, e_suit_opposite);
	dest->cards[0] = card_create(context, e_clubs, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination is black.");
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination is black.");
	dest->cards[0] = card_create(context, e_hearts, 2);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when source is red.");
	dest->cards[0] = card_create(context, e_diamonds, 2);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when source is red.");

	deck->cards[0] = card_create(context, e_hearts, 3);
	dest->cards[0] = card_create(context, e_clubs, 2);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination is black.");
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination is black.");
	dest->cards[0] = card_create(context, e_hearts, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when source is red.");
	dest->cards[0] = card_create(context, e_diamonds, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when source is red.");

	/* e_dest_higher */
	cond = condition_top_card_compare(context, dest,
									  e_dest_higher_value|e_follow_suit);
	deck->cards[0] = card_create(context, e_spades, 3);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination contains higher value.");

	/* e_dest_1higher */
	cond = condition_top_card_compare(context, dest,
									  e_dest_1higher_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination contains higher value.");

	dest->cards[0] = card_create(context, e_spades, 5);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains higher value"
				  " more than one step.");

	/* e_dest_lower */
	cond = condition_top_card_compare(context, dest,
									  e_dest_lower_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains higher value.");

	/* e_dest_1lower */
	cond = condition_top_card_compare(context, dest,
									  e_dest_1lower_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains higher value.");

	dest->cards[0] = card_create(context, e_spades, 1);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains lower value more than 1 step.");

	/* e_equal_value */
	cond = condition_top_card_compare(context, dest, e_equal_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains lower value.");

	dest->cards[0] = card_create(context, e_spades, 4);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains higher value.");

	dest->cards[0] = card_create(context, e_spades, 3);
	ck_assert_msg(cond->check(cond, &action) == true,
				  "true when destination contains equal value.");

	/* Use destination from move_action */
	cond = condition_top_card_compare(context, 0, e_equal_value|e_follow_suit);
	dest->cards[0] = card_create(context, e_spades, 2);
	ck_assert_msg(cond->check(cond, &action) == false,
				  "false when destination contains lower value.");
}
END_TEST

START_TEST(test_ruleset_get_move_action) {
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

	action = ruleset_get_move_action(vis, vis->piles[0]->cards[4], 3, vis->piles[1]);

	ck_assert_msg(action != 0, "No action returned.");
	ck_assert_msg(action->source == deck, "Deck should be the source.");
	ck_assert_msg(action->source_index == 4, "Source index incorrect.");
	ck_assert_msg(action->source_count == 3, "Source count incorrect.");
	ck_assert_msg(action->destination == done, "Done should be destination.");
	ck_assert_msg(action->destination_index == 0, "Destination index incorrect.");
}
END_TEST

START_TEST(test_ruleset_apply_move_action) {
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

	action = ruleset_get_move_action(vis, vis->piles[0]->cards[51], 1, vis->piles[1]);
	ruleset_apply_move_action(vis, action);
	visual_sync(vis);

	ck_assert_msg(vis->piles[0]->card_count == 51, "Card count should be one less.");
	ck_assert_msg(vis->piles[1]->card_count == 1, "Card count should be one more.");
	ck_assert_msg(vis->piles[0]->cards[51] == 0, "Card should have been moved.");
	ck_assert_msg(vis->piles[1]->cards[0] != 0, "Card should have been moved here.");
}
END_TEST

START_TEST(test_ruleset_apply_move_action2) {
	pile *deck, *done;
	visual *vis;
	move_action action;
	mem_context *context = mem_context_create();
	int index;

	deck = pile_create(context, 52);
	done = pile_create(context, 52);
	create_deck(context, deck, 1);

	vis = visual_create(context, 0);
	visual_add_pile(context, vis, visual_pile_create(context, deck));
	visual_add_pile(context, vis, visual_pile_create(context, done));
	visual_sync(vis);

	action.source = deck;
	action.source_index = 1;
	action.source_count = 5;
	action.destination = done;
	action.destination_index = 0;

	ck_assert_msg(deck->cards[0]->value == 1, "Should be an ace here.");
	ck_assert_msg(deck->cards[0]->suit == e_clubs, "Should be clubs.");

	ruleset_apply_move_action(vis, &action);
	visual_sync(vis);

	ck_assert_msg(card_count(deck) == 47,
				  "Number of cards should be 47 after move. But was %d", card_count(deck));
	ck_assert_msg(deck->cards[0]->value == 1, "Should be an ace here.");
	ck_assert_msg(deck->cards[0]->suit == e_clubs, "Should be clubs.");
	ck_assert_msg(deck->cards[1]->value == 7, "Should be an 7 here.");
	ck_assert_msg(deck->cards[1]->suit == e_clubs, "Should be clubs.");
	for(index=0;index<5;++index) {
		ck_assert_msg(done->cards[index]->value == index + 2,
					  "Should be an card here.");
		ck_assert_msg(done->cards[index]->suit == e_clubs, "Should be clubs.");
	}
}
END_TEST

START_TEST(test_ruleset_apply_move_action3) {
	pile *deck;
	visual *vis;
	move_action action;
	mem_context *context = mem_context_create();

	deck = pile_create(context, 52);
	deck->cards[0] = card_create(context, e_clubs, 1);
	deck->cards[1] = card_create(context, e_clubs, 2);

	vis = visual_create(context, 0);
	visual_add_pile(context, vis, visual_pile_create(context, deck));
	visual_sync(vis);

	action.source = deck;
	action.source_index = 1;
	action.source_count = 1;
	action.destination = deck;
	action.destination_index = 2;

	ruleset_apply_move_action(vis, &action);
	ck_assert(deck->cards[0] != 0);
	ck_assert(deck->cards[1] != 0);
	ck_assert(deck->cards[0]->value == 1);
	ck_assert(deck->cards[1]->value == 2);
}
END_TEST

START_TEST(test_action_reveal_source_top_card) {
	pile *deck;
	mem_context *context = mem_context_create();
	rule_action *action;
	move_action move;

	deck = pile_create(context, 52);
	create_deck(context, deck, 1);
	move.source = deck;

	action = action_reveal_source_top_card(context);
	action->execute(action, &move);

	ck_assert_msg(deck->cards[51]->proxy->card != 0, "Card should have been revealed");
}
END_TEST

void add_rule_tests(Suite *suite) {
	TCase *c = tcase_create("Rules");
	tcase_add_test(c, test_condition_card_count_array);
	tcase_add_test(c, test_condition_source);
	tcase_add_test(c, test_condition_source_array);
	tcase_add_test(c, test_condition_source_card_revealed);
	tcase_add_test(c, test_condition_source_not_equal_destination);
	tcase_add_test(c, test_condition_move_count);
	tcase_add_test(c, test_condition_or);
	tcase_add_test(c, test_condition_or_array);
	tcase_add_test(c, test_condition_destination);
	tcase_add_test(c, test_condition_destination_array);
	tcase_add_test(c, test_condition_destination_empty);
	tcase_add_test(c, test_condition_rest_of_pile);
	tcase_add_test(c, test_condition_top_card);
	tcase_add_test(c, test_condition_card_equal);
	tcase_add_test(c, test_condition_top_card_compare);
	tcase_add_test(c, test_rule_create);
	tcase_add_test(c, test_rule_add_condition);
	tcase_add_test(c, test_rule_add_action);
	tcase_add_test(c, test_rule_check);
	tcase_add_test(c, test_rule_execute_actions);
	tcase_add_test(c, test_ruleset_create);
	tcase_add_test(c, test_ruleset_add_rule);
	tcase_add_test(c, test_ruleset_check);
	tcase_add_test(c, test_ruleset_move_card);
	tcase_add_test(c, test_ruleset_move_individual_card);
	tcase_add_test(c, test_ruleset_get_move_action);
	tcase_add_test(c, test_ruleset_apply_move_action);
	tcase_add_test(c, test_ruleset_apply_move_action2);
	tcase_add_test(c, test_ruleset_apply_move_action3);
	tcase_add_test(c, test_action_reveal_source_top_card);
	suite_add_tcase(suite, c);
}
