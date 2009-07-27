#include <check.h>
#include <stdlib.h>
#include "test_card.h"
#include "test_sol_theidiot.h"

int main(int argc, char* argv[]) {
	int failed;
	TCase *card_case, *sol_theidiot;
	SRunner* runner;

	Suite* suite = suite_create("Cardy");

	card_case = tcase_create("Card");
	tcase_add_test(card_case, test_create_deck);
	tcase_add_test(card_case, test_card_count);
	tcase_add_test(card_case, test_card_take_last);
	tcase_add_test(card_case, test_card_append);
	tcase_add_test(card_case, test_card_append_all);
	tcase_add_test(card_case, test_card_first_free);
	tcase_add_test(card_case, test_pile_create);
	tcase_add_test(card_case, test_visual_create);
	tcase_add_test(card_case, test_visual_add_pile);
	suite_add_tcase(suite, card_case);

	sol_theidiot = tcase_create("Sol-TheIdiot");
	tcase_add_test(sol_theidiot, test_sol_theidiot_init);
	tcase_add_test(sol_theidiot, test_sol_theidiot_deal);
	suite_add_tcase(suite, sol_theidiot);

	runner = srunner_create(suite);
	srunner_run_all(runner, CK_VERBOSE);

	failed = srunner_ntests_failed(runner);
	srunner_free(runner);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
