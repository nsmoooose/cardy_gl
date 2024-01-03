#include <check.h>
#include "../api/ease.h"

START_TEST(test_ease_quad_in) {
	ck_assert(ease_quad_in(0.0f, 0.1f, 3.0f, 1.0f) == 0.1f);
	ck_assert(ease_quad_in(5.0f, 0.0f, 3.0f, 5.0f) == 3.0f);
}
END_TEST

START_TEST(test_ease_linear) {
	ck_assert(ease_linear(0.0f, 0.1f, 3.0f, 1.0f) == 0.1f);
	ck_assert(ease_linear(5.0f, 0.0f, 3.0f, 5.0f) == 3.0f);
	ck_assert(ease_linear(2.5f, 0.0f, 3.0f, 5.0f) == 1.5f);
}
END_TEST

void add_ease_tests(Suite *suite) {
	TCase *c = tcase_create("Ease");
	tcase_add_test(c, test_ease_quad_in);
	tcase_add_test(c, test_ease_linear);
	suite_add_tcase(suite, c);
}
