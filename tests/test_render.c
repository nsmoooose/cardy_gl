#include <check.h>
#include <stdlib.h>
#include "../lib/render.h"

START_TEST(test_render_context_create) {
	render_context *c = render_context_create(0);

	ck_assert_msg(c->object == 0, "Root object should be 0.");
	ck_assert_msg(c->selection_size == 0, "No selections has been created yet.");
}
END_TEST

START_TEST(test_render_object_create) {
	render_object *o = render_object_create("id");

	ck_assert_msg(o->id != 0, "Should be set.");
	ck_assert_msg(strcmp(o->id, "id") == 0, "Should be equal.");
	ck_assert_msg(o->data == 0, "Data should be set to zero.");
	ck_assert_msg(o->render == 0, "No render imp should be set.");
	ck_assert_msg(o->parent == 0, "No parent should be set.");
	ck_assert_msg(o->children == 0, "No children assigned.");
	ck_assert_msg(o->child_count == 0, "Child count should be 0");
}
END_TEST

START_TEST(test_render_object_add_child) {
	render_object *parent = render_object_create("");
	render_object *child1 = render_object_create("");
	render_object *child2 = render_object_create("");

	render_object_add_child(parent, child1);
	render_object_add_child(parent, child2);

	ck_assert_msg(parent->children[0] == child1, "Child1 not set");
	ck_assert_msg(parent->children[1] == child2, "Child2 not set");
	ck_assert_msg(parent->child_count == 2, "Child count is not correct.");
	ck_assert_msg(parent == child1->parent, "Parent not set.");
}
END_TEST

START_TEST(test_render_object_remove_child) {
	render_object *parent = render_object_create("");
	render_object *child1 = render_object_create("");
	render_object *child2 = render_object_create("");

	render_object_add_child(parent, child1);
	render_object_add_child(parent, child2);

	render_object_remove_child(parent, child1);
	ck_assert_msg(parent->child_count == 1, "Child count incorrect.");
	ck_assert_msg(parent->children[0] == child2, "Incorrect child.");
	ck_assert_msg(child1->parent == 0, "Parent not reset when removed.");

	render_object_remove_child(parent, child2);
	ck_assert_msg(parent->child_count == 0, "Child count should be 0");
	ck_assert_msg(parent->children == 0, "Child vector should have been freed.");
}
END_TEST

START_TEST(test_render_object_find) {
	render_object *root = render_object_create("root");
	render_object *child1 = render_object_create("child1");
	render_object *child2 = render_object_create("child2");
	render_object *subchild1 = render_object_create("subchild1");
	render_object *subchild2 = render_object_create("subchild2");
	render_object *noid = render_object_create(0);

	render_object_add_child(root, noid);
	render_object_add_child(root, child1);
	render_object_add_child(root, child2);
	render_object_add_child(child1, subchild1);
	render_object_add_child(child2, subchild2);

	ck_assert_msg(render_object_find(root, "child1") == child1, "Child1 not found.");
	ck_assert_msg(render_object_find(root, "child2") == child2, "Child2 not found.");
	ck_assert_msg(render_object_find(root, "subchild1") == subchild1,
				  "Subchild1 not found.");
	ck_assert_msg(render_object_find(root, "subchild2") == subchild2,
				  "Subchild2 not found.");
	ck_assert_msg(render_object_find(root, "root") == root, "root not found.");
}
END_TEST

START_TEST(test_render_object_find_root) {
	render_object *root = render_object_create("root");
	render_object *child1 = render_object_create("child1");
	render_object *child2 = render_object_create("child2");
	render_object *subchild1 = render_object_create("subchild1");
	render_object *subchild2 = render_object_create("subchild2");
	render_object *noid = render_object_create(0);

	render_object_add_child(root, noid);
	render_object_add_child(root, child1);
	render_object_add_child(root, child2);
	render_object_add_child(child1, subchild1);
	render_object_add_child(child2, subchild2);

	ck_assert_msg(render_object_find_root(subchild2) == root, "Root not found.");
}
END_TEST

void add_render_tests(Suite *suite) {
	TCase *c = tcase_create("Render");
	tcase_add_test(c, test_render_context_create);
	tcase_add_test(c, test_render_object_create);
	tcase_add_test(c, test_render_object_add_child);
	tcase_add_test(c, test_render_object_remove_child);
	tcase_add_test(c, test_render_object_find);
	tcase_add_test(c, test_render_object_find_root);
	suite_add_tcase(suite, c);
}