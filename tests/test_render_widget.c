#include <check.h>
#include <stdlib.h>
#include "../api/render_widget.h"

START_TEST(test_widget_get_default_style) {
	render_object *ro = widget_generic(0);
	widget_style *style = widget_get_default_style(ro);

	ck_assert(style != 0);
}
END_TEST

START_TEST(test_widget_set_pos) {
	expression *left, *top;
	render_object *ro = widget_generic(0);
	widget_style *style = widget_get_default_style(ro);
	expression_context *ec = widget_style_get_expression_context(style);

	widget_style_set_pos(style, 1.0f, 2.0f);
	left = widget_style_get_left_expression(style);
	top = widget_style_get_top_expression(style);

	ck_assert(left != 0);
	ck_assert(top != 0);
	ck_assert(expression_execute(ec, left) == 1.0f);
	ck_assert(expression_execute(ec, top) == 2.0f);
}
END_TEST

START_TEST(test_widget_set_size) {
	expression *width, *height;
	render_object *ro = widget_generic(0);
	widget_style *style = widget_get_default_style(ro);
	expression_context *ec = widget_style_get_expression_context(style);

	widget_style_set_size(style, 1.0f, 2.0f);
	width = widget_style_get_width_expression(style);
	height = widget_style_get_height_expression(style);

	ck_assert(width != 0);
	ck_assert(height != 0);
	ck_assert(expression_execute(ec, width) == 1.0f);
	ck_assert(expression_execute(ec, height) == 2.0f);
}
END_TEST

START_TEST(test_widget_object_variable_render_time) {
	render_object *ro = widget_generic(0);
	widget_style *style = widget_get_default_style(ro);
	expression_context *ec = widget_style_get_expression_context(style);
	expression *e = expression_context_get(ec, style_key_render_time);

	ro->render_time = 3.1f;

	ck_assert(e != 0);
	ck_assert(expression_execute(ec, e) == 3.1f);
}
END_TEST

START_TEST(test_widget_set_data) {
	render_object *ro = widget_generic(0);
	widget_set_data(ro, ro);

	ck_assert(widget_get_data(ro) == ro);
}
END_TEST

void add_render_widget_tests(Suite *suite) {
	TCase *c = tcase_create("Render widget");
	tcase_add_test(c, test_widget_get_default_style);
	tcase_add_test(c, test_widget_set_pos);
	tcase_add_test(c, test_widget_set_size);
	tcase_add_test(c, test_widget_object_variable_render_time);
	tcase_add_test(c, test_widget_set_data);
	suite_add_tcase(suite, c);
}
