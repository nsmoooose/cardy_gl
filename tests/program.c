#include <check.h>
#include <stdlib.h>

void add_memory_tests(Suite *suite);
void add_card_tests(Suite *suite);
void add_rule_tests(Suite *suite);
void add_theme_tests(Suite *suite);
void add_render_tests(Suite *suite);
void add_sol_heirship_tests(Suite *suite);
void add_sol_theidiot_tests(Suite *suite);
void add_sol_malteser_tests(Suite *suite);
void add_sol_noname1_tests(Suite *suite);
void add_sol_test1_tests(Suite *suite);
void add_ease_tests(Suite *suite);
void add_expression_tests(Suite *suite);
void add_expression_lib_tests(Suite *suite);
void add_render_widget_tests(Suite *suite);
void add_game_registry_tests(Suite *suite);
void add_game_system_tests(Suite *suite);
void add_network_tests(Suite *suite);

int main(int argc, char* argv[]) {
	int failed;
	SRunner* runner;

	Suite* suite = suite_create("Cardy");
	add_memory_tests(suite);
	add_card_tests(suite);
	add_rule_tests(suite);
	add_theme_tests(suite);
	add_render_tests(suite);
	add_sol_heirship_tests(suite);
	add_sol_theidiot_tests(suite);
	add_sol_malteser_tests(suite);
	add_sol_noname1_tests(suite);
	add_sol_test1_tests(suite);
	add_ease_tests(suite);
	add_expression_tests(suite);
	add_expression_lib_tests(suite);
	add_render_widget_tests(suite);
	add_game_registry_tests(suite);
	add_game_system_tests(suite);
	add_network_tests(suite);

	runner = srunner_create(suite);
	srunner_run_all(runner, CK_VERBOSE);

	failed = srunner_ntests_failed(runner);
	srunner_free(runner);

	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
