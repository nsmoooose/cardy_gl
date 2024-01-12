#include <check.h>
#include <stdlib.h>
#include "api/render.h"
#include "api/render_widget.h"

START_TEST(test_render_context_create) {
	render_context *c = render_context_create();

	ck_assert_msg(c->object == 0, "Root object should be 0.");
	ck_assert_msg(c->selection_size == 0,
	              "No selections has been created yet.");
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

	render_object_remove_child(child1);
	ck_assert_msg(parent->child_count == 1, "Child count incorrect.");
	ck_assert_msg(parent->children[0] == child2, "Incorrect child.");
	ck_assert_msg(child1->parent == 0, "Parent not reset when removed.");

	render_object_remove_child(child2);
	ck_assert_msg(parent->child_count == 0, "Child count should be 0");
	ck_assert_msg(parent->children == 0,
	              "Child vector should have been freed.");
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

	ck_assert_msg(render_object_find(root, "child1") == child1,
	              "Child1 not found.");
	ck_assert_msg(render_object_find(root, "child2") == child2,
	              "Child2 not found.");
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

	ck_assert_msg(render_object_find_root(subchild2) == root,
	              "Root not found.");
}
END_TEST

unsigned char kd_key = 0;
int kd_mod = 0;
int kd_x = 0;
int kd_y = 0;

bool callback_keydown(render_event_args *event, unsigned char key,
                      int modifiers, int x, int y) {
	if (key == 'b') {
		kd_key = key;
		kd_mod = modifiers;
		kd_x = x;
		kd_y = y;
		return true;
	}
	return false;
}

START_TEST(test_render_process_keyboard_down) {
	render_context *rc = render_context_create();
	render_object *desktop = widget_desktop("desktop");
	render_object *window = widget_generic("menu");

	rc->object = desktop;
	render_object_add_child(desktop, window);
	window->keyboard_down = callback_keydown;

	render_process_keyboard_down(rc, 'a', 1, 2, 3);
	ck_assert(kd_key == 0);

	render_process_keyboard_down(rc, 'b', 1, 2, 3);

	ck_assert(kd_key == 'b');
	ck_assert(kd_mod == 1);
	ck_assert(kd_x = 2);
	ck_assert(kd_y = 3);
}
END_TEST

unsigned char ku_key = 0;
int ku_mod = 0;
int ku_x = 0;
int ku_y = 0;

bool callback_keyup(render_event_args *event, unsigned char key, int modifiers,
                    int x, int y) {
	if (key == 'b') {
		ku_key = key;
		ku_mod = modifiers;
		ku_x = x;
		ku_y = y;
		return true;
	}
	return false;
}

START_TEST(test_render_process_keyboard_up) {
	render_context *rc = render_context_create();
	render_object *desktop = widget_desktop("desktop");
	render_object *window = widget_generic("menu");

	rc->object = desktop;
	render_object_add_child(desktop, window);
	window->keyboard_up = callback_keyup;

	render_process_keyboard_up(rc, 'a', 1, 2, 3);
	ck_assert(ku_key == 0);

	render_process_keyboard_up(rc, 'b', 1, 2, 3);

	ck_assert(ku_key == 'b');
	ck_assert(ku_mod == 1);
	ck_assert(ku_x = 2);
	ck_assert(ku_y = 3);
}
END_TEST

unsigned char skd_key = 0;
int skd_mod = 0;
int skd_x = 0;
int skd_y = 0;

bool callback_special_keydown(render_event_args *event, int key, int modifiers,
                              int x, int y) {
	if (key == 'b') {
		skd_key = key;
		skd_mod = modifiers;
		skd_x = x;
		skd_y = y;
		return true;
	}
	return false;
}

START_TEST(test_render_process_keyboard_special_down) {
	render_context *rc = render_context_create();
	render_object *desktop = widget_desktop("desktop");
	render_object *window = widget_generic("menu");

	rc->object = desktop;
	render_object_add_child(desktop, window);
	window->keyboard_special_down = callback_special_keydown;

	render_process_keyboard_special_down(rc, 'a', 1, 2, 3);
	ck_assert(skd_key == 0);

	render_process_keyboard_special_down(rc, 'b', 1, 2, 3);

	ck_assert(skd_key == 'b');
	ck_assert(skd_mod == 1);
	ck_assert(skd_x = 2);
	ck_assert(skd_y = 3);
}
END_TEST

unsigned char sku_key = 0;
int sku_mod = 0;
int sku_x = 0;
int sku_y = 0;

bool callback_special_keyup(render_event_args *event, int key, int modifiers,
                            int x, int y) {
	if (key == 'b') {
		sku_key = key;
		sku_mod = modifiers;
		sku_x = x;
		sku_y = y;
		return true;
	}
	return false;
}

START_TEST(test_render_process_keyboard_special_up) {
	render_context *rc = render_context_create();
	render_object *desktop = widget_desktop("desktop");
	render_object *window = widget_generic("menu");

	rc->object = desktop;
	render_object_add_child(desktop, window);
	window->keyboard_special_up = callback_special_keyup;

	render_process_keyboard_special_up(rc, 'a', 1, 2, 3);
	ck_assert(ku_key == 0);

	render_process_keyboard_special_up(rc, 'b', 1, 2, 3);

	ck_assert(sku_key == 'b');
	ck_assert(sku_mod == 1);
	ck_assert(sku_x = 2);
	ck_assert(sku_y = 3);
}
END_TEST

START_TEST(test_render_time) {
	render_context *rc = render_context_create();
	render_object *ro = render_object_create(0);

	render_scene_object(rc, ro, 1.0f);
	ck_assert(ro->render_time == 0.0f);

	render_scene_object(rc, ro, 2.0f);
	ck_assert(ro->render_time == 2.0f);

	render_scene_object(rc, ro, 3.0f);
	ck_assert(ro->render_time == 5.0f);
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
	tcase_add_test(c, test_render_process_keyboard_down);
	tcase_add_test(c, test_render_process_keyboard_up);
	tcase_add_test(c, test_render_process_keyboard_special_down);
	tcase_add_test(c, test_render_process_keyboard_special_up);
	tcase_add_test(c, test_render_time);
	suite_add_tcase(suite, c);
}
