#ifndef __EXPRESSION_LIB_H__
#define __EXPRESSION_LIB_H__

#include "expression.h"
#include "types.h"

struct expression_lib_St;
typedef struct expression_lib_St expression_lib;

expression_lib *expression_lib_create();
void expression_lib_free(expression_lib *lib);

expression_lib *expression_lib_default();

void expression_lib_add_function1f(expression_lib *lib, const char *name, function1f fun);
void expression_lib_add_function2f(expression_lib *lib, const char *name, function2f fun);
void expression_lib_add_function3f(expression_lib *lib, const char *name, function3f fun);
void expression_lib_add_function4f(expression_lib *lib, const char *name, function4f fun);
void expression_lib_add_function5f(expression_lib *lib, const char *name, function5f fun);
void expression_lib_add_function6f(expression_lib *lib, const char *name, function6f fun);

bool expression_lib_is_function(expression_lib *lib, const char *name);

int expression_lib_param_count(expression_lib *lib, const char *name);

expression *expression_lib_build_expression(expression_lib *lib, const char *name, expression *params[]);

#endif /* __EXPRESSION_LIB_H__ */
