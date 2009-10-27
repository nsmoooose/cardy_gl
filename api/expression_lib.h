#ifndef __EXPRESSION_LIB_H__
#define __EXPRESSION_LIB_H__

#include "expression.h"
#include "types.h"

typedef struct {
		int dummy;
} expression_lib;

expression_lib *expression_lib_create();
void expression_lib_free(expression_lib *lib);

expression_lib *expression_lib_default();

void expression_lib_add_function1f(expression_lib *lib, const char *name, float (*fun)(float));
void expression_lib_add_function2f(expression_lib *lib, const char *name, float (*fun)(float, float));
void expression_lib_add_function3f(expression_lib *lib, const char *name, float (*fun)(float, float, float));
void expression_lib_add_function4f(expression_lib *lib, const char *name, float (*fun)(float, float, float, float));
void expression_lib_add_function5f(expression_lib *lib, const char *name, float (*fun)(float, float, float, float, float));
void expression_lib_add_function6f(expression_lib *lib, const char *name, float (*fun)(float, float, float, float, float, float));

bool expression_lib_is_function(expression_lib *lib, const char *name);

int expression_lib_param_count(expression_lib *lib, const char *name);

expression *expression_lib_build_expression(expression_lib *lib, const char *name, float params[]);

#endif /* __EXPRESSION_LIB_H__ */
