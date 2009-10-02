#include <check.h>
#include <stdlib.h>
#include "../api/expression.h"

START_TEST(test_expression_const) {
	mem_context *mc = mem_context_create();
	expression *e = expression_const(mc, 3.0f);
	expression_context *ec = expression_context_create(mc);

	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 3.0f);
}
END_TEST

void add_expression_tests(Suite *suite) {
	TCase *c = tcase_create("Expression");
	tcase_add_test(c, test_expression_const);
	suite_add_tcase(suite, c);
}
