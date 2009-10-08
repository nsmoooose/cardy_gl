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

START_TEST(test_expression_create_token) {
	expression_token *token = expression_create_token(e_type_var, "abcdefg*123", 7);

	ck_assert(token->type == e_type_var);
	ck_assert(strcmp(token->content, "abcdefg") == 0);
}
END_TEST

START_TEST(test_expression_tokenize) {
	expression_token **tokens = expression_tokenize("3+4");

	ck_assert(tokens != 0);
	ck_assert(expression_token_count(tokens) == 3);

	ck_assert(tokens[0] != 0);
	ck_assert(tokens[0]->type == e_type_const);
	ck_assert(strcmp(tokens[0]->content, "3") == 0);

	ck_assert(tokens[1] != 0);
	ck_assert(tokens[1]->type == (e_type_op|e_type_add));

	ck_assert(tokens[2] != 0);
	ck_assert(strcmp(tokens[2]->content, "4") == 0);

	tokens = expression_tokenize("3.0");
	ck_assert(expression_token_count(tokens) == 1);
	ck_assert(tokens[0]->type == e_type_const);

	tokens = expression_tokenize("apa");
	ck_assert(expression_token_count(tokens) == 1);
	ck_assert(tokens[0]->type == e_type_var);
	ck_assert(strcmp(tokens[0]->content, "apa") == 0);


	tokens = expression_tokenize("width*4");
	ck_assert(expression_token_count(tokens) == 3);
	ck_assert(tokens[0]->type == e_type_var);
	ck_assert(strcmp(tokens[0]->content, "width") == 0);
	ck_assert(tokens[1]->type == (e_type_op|e_type_mul));
	ck_assert(tokens[2]->type == e_type_const);
	ck_assert(strcmp(tokens[2]->content, "4") == 0);

	tokens = expression_tokenize("3+3*4");
	ck_assert(expression_token_count(tokens) == 5);
}
END_TEST

START_TEST(test_expression_parse) {
	expression_context *ec = expression_context_create();
	expression *e = expression_parse("3.0");

	mark_point();
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 3.0f);

	mark_point();
	e = expression_parse("3+4");
	ck_assert(expression_execute(ec, e) == 7.0f);

	mark_point();
	e = expression_parse("3-4");
	ck_assert(expression_execute(ec, e) == -1.0f);

	mark_point();
	e = expression_parse("3*4");
	ck_assert(expression_execute(ec, e) == 12.0f);

	e = expression_parse("3/4");
	ck_assert(expression_execute(ec, e) == 0.75f);
}
END_TEST

START_TEST(test_expression_parse_var) {
	expression_context *ec = expression_context_create();
	expression *e;

	expression_context_set(ec, "width", expression_const(3.0));

	e = expression_parse("width*4");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 12.0f);
}
END_TEST

START_TEST(test_expression_parse_prio1) {
	expression_context *ec = expression_context_create();
	expression *e;

	e = expression_parse("3+3+4");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 10.0f);

	e = expression_parse("3+3-2");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 4.0f);
}
END_TEST

START_TEST(test_expression_parse_prio2) {
	expression_context *ec = expression_context_create();
	expression *e;

	e = expression_parse("3+3*4");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 15.0f);

	e = expression_parse("3+3/4");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 3.75f);

	e = expression_parse("3*3*9");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 81.0f);
}
END_TEST

START_TEST(test_expression_parse3) {
	expression_context *ec = expression_context_create();
	expression *e;

	e = expression_parse("(3+3)*4");
	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 24.0f);
}
END_TEST

START_TEST(test_expression_parse_invalid_chars) {
	ck_assert(expression_parse("") == 0);
	ck_assert(expression_parse("3.3!\"#¤ %&") == 0);
	ck_assert(expression_parse("3+") == 0);
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
	tcase_add_test(c, test_expression_create_token);
	tcase_add_test(c, test_expression_tokenize);
	tcase_add_test(c, test_expression_parse);
	tcase_add_test(c, test_expression_parse_var);
	tcase_add_test(c, test_expression_parse_prio1);
	tcase_add_test(c, test_expression_parse_prio2);
	tcase_add_test(c, test_expression_parse3);
	tcase_add_test(c, test_expression_parse_invalid_chars);
	suite_add_tcase(suite, c);
}
