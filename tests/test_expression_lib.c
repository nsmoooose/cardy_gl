#include <check.h>
#include <math.h>
#include <stdlib.h>
#include "../api/expression.h"

START_TEST(test_expression_lib_add_function1f) {
	expression_lib *lib = expression_lib_create();

	expression_lib_add_function1f(lib, "sin", sinf);

	ck_assert(expression_lib_is_function(lib, "sin") == true);
	ck_assert(expression_lib_param_count(lib, "sin") == 1);
}
END_TEST

START_TEST(test_expression_lib_build_expression) {
	expression *e, *params[1];
	float v1, v2;
	expression_context *ec = expression_context_create();
	expression_lib *lib = expression_lib_create();
	expression_lib_add_function1f(lib, "sin", sinf);

	params[0] = expression_const(1.0f);
	e = expression_lib_build_expression(lib, "sin", params);

	ck_assert(e != 0);
	v1 = expression_execute(ec, e);
	v2 = sinf(1.0f);
	ck_assert(fabsf(v1 - v2) < 0.00001);
}
END_TEST

void add_expression_lib_tests(Suite *suite) {
	TCase *c = tcase_create("Expression lib");
	tcase_add_test(c, test_expression_lib_add_function1f);
	tcase_add_test(c, test_expression_lib_build_expression);
	suite_add_tcase(suite, c);
}
