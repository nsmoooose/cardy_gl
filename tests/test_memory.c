#include <check.h>
#include "../lib/memory.h"

START_TEST(test_context_create) {
	int index;
	mem_context *c = mem_context_create();

	ck_assert_msg(c != 0, "Failed to allocate context.");
	ck_assert_msg(c->blocks != 0, "The context is default non zero.");
	ck_assert_msg(c->size == 1000, "Default size is 1000");
	for(index=0;index<c->size;++index) {
		ck_assert_msg(c->blocks[index] == 0, "Block wasn't 0 after creation.");
	}
}
END_TEST

START_TEST(test_alloc) {
	void *data;
	mem_context *c = mem_context_create();

	data = mem_alloc(c, 100);
	ck_assert_msg(data != 0, "Allocation failed.");
	ck_assert_msg(c->blocks[0] == data, "First block wasn't set.");
	ck_assert_msg(c->blocks[1] == 0, "Second block is still free.");

	data = mem_alloc(c, 100);
	ck_assert_msg(c->blocks[1] == data, "Second block wasn't set.");
}
END_TEST

START_TEST(test_reallocs) {
	int index;
	void *data;
	mem_context *c = mem_context_create();

	for(index=0;index<1200;++index) {
		data = mem_alloc(c, 2);

		ck_assert_msg(data != 0, "Allocation failure.");
		ck_assert_msg(c->blocks[index] == data, "Failed to resize array and set block.");
	}
}
END_TEST

START_TEST(test_free) {
	void *data;
	mem_context *c = mem_context_create();

	data = mem_alloc(c, 39);
	mem_free(c, data);

	ck_assert_msg(c->blocks[0] == 0, "Block wasn't freed.");
}
END_TEST

START_TEST(test_context_free) {
	void *data;
	mem_context *c = mem_context_create();
	data = mem_alloc(c, 33);
	mem_context_free(c);
}
END_TEST

void add_memory_tests(Suite *suite) {
	TCase *c = tcase_create("Memory");
	tcase_add_test(c, test_context_create);
	tcase_add_test(c, test_alloc);
	tcase_add_test(c, test_reallocs);
	tcase_add_test(c, test_free);
	tcase_add_test(c, test_context_free);
	suite_add_tcase(suite, c);
}
