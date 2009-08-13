#include <check.h>
#include <stdlib.h>
#include "test_card.h"
#include "test_rule.h"
#include "test_sol_theidiot.h"
#include "test_sol_maltesercross.h"
#include "test_sol_noname1.h"

int main(int argc, char* argv[]) {
	int failed;
	TCase *card_case, *rule_case, *sol_theidiot, *sol_malteser, *sol_noname1;
	SRunner* runner;

	Suite* suite = suite_create("Cardy");

	card_case = tcase_create("Card");
	tcase_add_test(card_case, test_card_create);
	tcase_add_test(card_case, test_create_deck);
	tcase_add_test(card_case, test_card_count);
	tcase_add_test(card_case, test_card_take_last);
	tcase_add_test(card_case, test_card_append);
	tcase_add_test(card_case, test_card_move_all);
	tcase_add_test(card_case, test_card_first_free);
	tcase_add_test(card_case, test_vis_pile_create);
	tcase_add_test(card_case, test_visual_create);
	tcase_add_test(card_case, test_visual_add_pile);
	tcase_add_test(card_case, test_visual_sync);
	tcase_add_test(card_case, test_card_reveal);
	tcase_add_test(card_case, test_card_reveal_count);
	tcase_add_test(card_case, test_card_reveal_all);
	tcase_add_test(card_case, test_card_hide);
	tcase_add_test(card_case, test_card_hide_count);
	tcase_add_test(card_case, test_card_hide_all);
	suite_add_tcase(suite, card_case);

	rule_case = tcase_create("Rules");
	tcase_add_test(rule_case, test_condition_source);
	tcase_add_test(rule_case, test_create_rule);
	tcase_add_test(rule_case, test_rule_add_condition);
	tcase_add_test(rule_case, test_rule_check);
	tcase_add_test(rule_case, test_create_ruleset);
	tcase_add_test(rule_case, test_ruleset_add_rule);
	tcase_add_test(rule_case, test_ruleset_check);
	suite_add_tcase(suite, rule_case);

	sol_theidiot = tcase_create("Sol-TheIdiot");
	tcase_add_test(sol_theidiot, test_sol_theidiot_init);
	tcase_add_test(sol_theidiot, test_sol_theidiot_deal);
	tcase_add_test(sol_theidiot, test_sol_theidiot_moving_card);
	suite_add_tcase(suite, sol_theidiot);

	sol_malteser = tcase_create("Sol-MalteserCross");
	tcase_add_test(sol_malteser, test_sol_maltesercross_init);
	tcase_add_test(sol_malteser, test_sol_maltesercross_deal);
	suite_add_tcase(suite, sol_malteser);

	sol_noname1 = tcase_create("Sol-Noname1");
	tcase_add_test(sol_noname1, test_sol_noname1_init);
	tcase_add_test(sol_noname1, test_sol_noname1_deal);
	tcase_add_test(sol_noname1, test_sol_noname1_deal2);
	suite_add_tcase(suite, sol_noname1);

	runner = srunner_create(suite);
	srunner_run_all(runner, CK_VERBOSE);

	failed = srunner_ntests_failed(runner);
	srunner_free(runner);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
