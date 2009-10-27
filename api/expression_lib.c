#include <math.h>
#include "expression_lib.h"

expression_lib *expression_lib_create() {
	return 0;
}

void expression_lib_free(expression_lib *lib) {
}

static void expression_lib_add_math_h(expression_lib *lib) {
	expression_lib_add_function1f(lib, "cos", cosf);
	expression_lib_add_function1f(lib, "sin", sinf);
	expression_lib_add_function1f(lib, "acos", acosf);
	expression_lib_add_function1f(lib, "asin", asinf);
}

static void expression_lib_add_ease_h(expression_lib *lib) {
}

expression_lib *expression_lib_default() {
	expression_lib *lib = expression_lib_create();
	expression_lib_add_math_h(lib);
	expression_lib_add_ease_h(lib);
	return lib;
}

void expression_lib_add_function1f(expression_lib *lib, const char *name, float (*fun)(float)) {
}

void expression_lib_add_function2f(expression_lib *lib, const char *name, float (*fun)(float, float)) {
}

void expression_lib_add_function3f(expression_lib *lib, const char *name, float (*fun)(float, float, float)) {
}

void expression_lib_add_function4f(expression_lib *lib, const char *name, float (*fun)(float, float, float, float)) {
}

void expression_lib_add_function5f(expression_lib *lib, const char *name, float (*fun)(float, float, float, float, float)) {
}

void expression_lib_add_function6f(expression_lib *lib, const char *name, float (*fun)(float, float, float, float, float, float)) {
}

bool expression_lib_is_function(expression_lib *lib, const char *name) {
	return false;
}

int expression_lib_param_count(expression_lib *lib, const char *name) {
	return -1;
}

expression *expression_lib_build_expression(expression_lib *lib, const char *name, float params[]) {
	return 0;
}
