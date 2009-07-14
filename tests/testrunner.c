#include <check.h>
#include <stdlib.h>
#include "test_card.h"
#include "test_sol_theidiot.h"

int main(int argc, char argv[]) {
	int failed;

	Suite* suite = suite_create("Cardy");

	TCase* card_case = tcase_create("Card");
	tcase_add_test(card_case, test_create_deck);
	suite_add_tcase(suite, card_case);

	TCase* sol_theidiot = tcase_create("Sol-TheIdiot");
	tcase_add_test(sol_theidiot, test_sol_theidiot_init);
	suite_add_tcase(suite, sol_theidiot);

	SRunner* runner = srunner_create(suite);
	srunner_run_all(runner, CK_VERBOSE);

	// failed = srunner_ntest_failed(runner);
	srunner_free(runner);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
