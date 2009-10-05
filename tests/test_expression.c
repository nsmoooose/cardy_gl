#include <check.h>
#include <stdlib.h>
#include "../api/expression.h"

START_TEST(test_expression_const) {
	expression *e = expression_const(3.0f);
	expression_context *ec = expression_context_create();

	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 3.0f);
}
END_TEST

START_TEST(test_expression_div) {
	expression *e1 = expression_const(3.0f);
	expression *e2 = expression_const(2.0f);
	expression *e3 = expression_div(e1, e2);
	expression_context *ec = expression_context_create();

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 1.5f);
}
END_TEST

START_TEST(test_expression_mult) {
	expression *e1 = expression_const(3.0f);
	expression *e2 = expression_const(2.0f);
	expression *e3 = expression_mult(e1, e2);
	expression_context *ec = expression_context_create();

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 6.0f);
}
END_TEST

START_TEST(test_expression_sub) {
	expression *e1 = expression_const(3.0f);
	expression *e2 = expression_const(2.0f);
	expression *e3 = expression_sub(e1, e2);
	expression_context *ec = expression_context_create();

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 1.0f);
}
END_TEST

START_TEST(test_expression_add) {
	expression *e1 = expression_const(3.0f);
	expression *e2 = expression_const(2.0f);
	expression *e3 = expression_add(e1, e2);
	expression_context *ec = expression_context_create();

	ck_assert(e3 != 0);
	ck_assert(expression_execute(ec, e3) == 5.0f);
}
END_TEST

START_TEST(test_expression_var) {
	expression *e1 = expression_const(3.0f);
	expression *e2 = expression_var("width");
	expression_context *ec = expression_context_create();
	expression_context_set(ec, "width", e1);

	ck_assert(e2 != 0);
	ck_assert(expression_execute(ec, e2) == 3.0f);
}
END_TEST

START_TEST(test_expression_context_set) {
	expression *e1 = expression_const(3.0f);
	expression *e2 = 0;
	expression_context *ec = expression_context_create();

	expression_context_set(ec, "a", e1);
	e2 = expression_context_get(ec, "a");

	ck_assert(e1 == e2);
}
END_TEST

START_TEST(test_expression_parse) {
	expression_context *ec = expression_context_create();
	expression *e = expression_parse("3.0");

	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 3.0f);

	e = expression_parse("3+4");
	ck_assert(expression_execute(ec, e) == 7.0f);

	e = expression_parse("3-4");
	ck_assert(expression_execute(ec, e) == -1.0f);

	e = expression_parse("3*4");
	ck_assert(expression_execute(ec, e) == 12.0f);

	e = expression_parse("3/4");
	ck_assert(expression_execute(ec, e) == 0.75f);
}
END_TEST

START_TEST(test_expression_parse2) {
	expression_context *ec = expression_context_create();
	expression *e;

	expression_context_set(ec, "width", expression_const(3.0));

	e = expression_parse("width*4");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 12.0f);
}
END_TEST

START_TEST(test_expression_parse_invalid_chars) {
	ck_assert(expression_parse("") == 0);
	ck_assert(expression_parse("3.3!\"#¤ %&") == 0);
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
	tcase_add_test(c, test_expression_parse);
	tcase_add_test(c, test_expression_parse2);
	tcase_add_test(c, test_expression_parse_invalid_chars);
	suite_add_tcase(suite, c);
}
