#include <math.h>
#include <stdlib.h>
#include "expression_lib.h"

typedef struct {
	int param_count;

	union {
		function1f f1;
		function2f f2;
		function3f f3;
		function4f f4;
		function5f f5;
		function6f f6;
	} function;
} expression_lib_function;

struct expression_lib_St {
	GHashTable *functions;
};

expression_lib *expression_lib_create() {
	expression_lib *lib = calloc(1, sizeof(expression_lib));
	lib->functions = g_hash_table_new(g_str_hash, g_str_equal);
	return lib;
}

void expression_lib_free(expression_lib *lib) {
	/* TODO
	   Iterate through all functions and free them.
	*/
	g_hash_table_unref(lib->functions);
	free(lib);
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

void expression_lib_add_function1f(expression_lib *lib, const char *name, function1f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 1;
	f->function.f1 = fun;
	g_hash_table_insert(lib->functions, (char*)name, f);
}

void expression_lib_add_function2f(expression_lib *lib, const char *name, function2f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 2;
	f->function.f2 = fun;
	g_hash_table_insert(lib->functions, (char*)name, f);
}

void expression_lib_add_function3f(expression_lib *lib, const char *name, function3f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 3;
	f->function.f3 = fun;
	g_hash_table_insert(lib->functions, (char*)name, f);
}

void expression_lib_add_function4f(expression_lib *lib, const char *name, function4f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 4;
	f->function.f4 = fun;
	g_hash_table_insert(lib->functions, (char*)name, f);
}

void expression_lib_add_function5f(expression_lib *lib, const char *name, function5f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 5;
	f->function.f5 = fun;
	g_hash_table_insert(lib->functions, (char*)name, f);
}

void expression_lib_add_function6f(expression_lib *lib, const char *name, function6f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 6;
	f->function.f6 = fun;
	g_hash_table_insert(lib->functions, (char*)name, f);
}

bool expression_lib_is_function(expression_lib *lib, const char *name) {
	return g_hash_table_lookup(lib->functions, name) != 0;
}

int expression_lib_param_count(expression_lib *lib, const char *name) {
	expression_lib_function *f = g_hash_table_lookup(lib->functions, name);
	return f->param_count;
}

expression *expression_lib_build_expression(expression_lib *lib, const char *name, expression *params[]) {
	expression_lib_function *f = g_hash_table_lookup(lib->functions, name);
	switch(f->param_count) {
	case 1:
		return expression_function1f(f->function.f1, params[0]);
	case 2:
		return expression_function2f(f->function.f2, params);
	case 3:
		return expression_function3f(f->function.f3, params);
	case 4:
		return expression_function4f(f->function.f4, params);
	case 5:
		return expression_function5f(f->function.f5, params);
	case 6:
		return expression_function6f(f->function.f6, params);
	default:
		return 0;
	}
}
