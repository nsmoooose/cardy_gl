#include <check.h>
#include <math.h>
#include <stdlib.h>
#include "../api/expression_lib.h"

START_TEST(test_expression_lib_add_function1f) {
	expression_lib *lib = expression_lib_create();

	expression_lib_add_function1f(lib, "sin", sinf);

	ck_assert(expression_lib_is_function(lib, "sin") == true);
	ck_assert(expression_lib_param_count(lib, "sin") == 1);
}
END_TEST

void add_expression_lib_tests(Suite *suite) {
	TCase *c = tcase_create("Expression lib");
	tcase_add_test(c, test_expression_lib_add_function1f);
	suite_add_tcase(suite, c);
}
