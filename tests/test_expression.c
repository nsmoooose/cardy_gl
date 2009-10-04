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

START_TEST(test_expression_div) {
	mem_context *mc = mem_context_create();
	expression *e1 = expression_const(mc, 3.0f);
	expression *e2 = expression_const(mc, 2.0f);
	expression *e3 = expression_div(mc, e1, e2);
	expression_context *ec = expression_context_create(mc);

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 1.5f);
}
END_TEST

START_TEST(test_expression_mult) {
	mem_context *mc = mem_context_create();
	expression *e1 = expression_const(mc, 3.0f);
	expression *e2 = expression_const(mc, 2.0f);
	expression *e3 = expression_mult(mc, e1, e2);
	expression_context *ec = expression_context_create(mc);

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 6.0f);
}
END_TEST

START_TEST(test_expression_sub) {
	mem_context *mc = mem_context_create();
	expression *e1 = expression_const(mc, 3.0f);
	expression *e2 = expression_const(mc, 2.0f);
	expression *e3 = expression_sub(mc, e1, e2);
	expression_context *ec = expression_context_create(mc);

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 1.0f);
}
END_TEST

START_TEST(test_expression_add) {
	mem_context *mc = mem_context_create();
	expression *e1 = expression_const(mc, 3.0f);
	expression *e2 = expression_const(mc, 2.0f);
	expression *e3 = expression_add(mc, e1, e2);
	expression_context *ec = expression_context_create(mc);

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 5.0f);
}
END_TEST

START_TEST(test_expression_var) {
	mem_context *mc = mem_context_create();
	expression *e1 = expression_const(mc, 3.0f);
	expression *e2 = expression_var(mc, "width");
	expression_context *ec = expression_context_create(mc);
	expression_context_set(ec, "width", e1);

	ck_assert(e2 != 0);
	ck_assert(expression_execute(ec, e2) == 3.0f);
}
END_TEST

START_TEST(test_expression_context_set) {
	mem_context *mc = mem_context_create();
	expression *e1 = expression_const(mc, 3.0f);
	expression *e2 = 0;
	expression_context *ec = expression_context_create(mc);

	expression_context_set(ec, "a", e1);
	e2 = expression_context_get(ec, "a");

	ck_assert(e1 == e2);
}
END_TEST

void add_expression_tests(Suite *suite) {
	TCase *c = tcase_create("Expression");
	tcase_add_test(c, test_expression_const);
	tcase_add_test(c, test_expression_div);
	tcase_add_test(c, test_expression_mult);
	tcase_add_test(c, test_expression_sub);
	tcase_add_test(c, test_expression_add);
	tcase_add_test(c, test_expression_var);
	tcase_add_test(c, test_expression_context_set);
	suite_add_tcase(suite, c);
}
