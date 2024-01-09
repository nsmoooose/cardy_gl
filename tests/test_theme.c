#include <check.h>
#include "api/theme.h"

START_TEST(test_theme_list) {
	int i;
	mem_context *context = mem_context_create();
	themes *t = theme_list(context, "card_themes");

	ck_assert_msg(t != 0, "No themes struct returned.");
	ck_assert_msg(t->theme_count == 5, "Incorrect number of themes listed.");

	for (i = 0; i < 5; ++i) {
		ck_assert_msg(strcmp(t->theme_names[i], "ancient_french") == 0 ||
		                  strcmp(t->theme_names[i], "anglo") == 0 ||
		                  strcmp(t->theme_names[i], "atlasnye") == 0 ||
		                  strcmp(t->theme_names[i], "dondorf") == 0 ||
		                  strcmp(t->theme_names[i], "paris") == 0,
		              "Not a matching theme name.");
	}
}
END_TEST

START_TEST(test_theme_list_invalid_dir) {
	mem_context *context = mem_context_create();
	themes *t = theme_list(context, "ajkfhaslkf");

	ck_assert_msg(t == 0, "Themes should be 0");
	ck_assert_msg(context->blocks[0] == 0, "No allocation should be made.");
}
END_TEST

void add_theme_tests(Suite *suite) {
	TCase *cases = tcase_create("Card");
	tcase_add_test(cases, test_theme_list);
	tcase_add_test(cases, test_theme_list_invalid_dir);
	suite_add_tcase(suite, cases);
}
