#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include <glib.h>
#include "memory.h"

struct expression_context_St;
typedef struct expression_context_St expression_context;

struct expression_St;
typedef struct expression_St expression;

expression_context *expression_context_create(mem_context *mc);

expression *expression_const(mem_context *mc, float value);

expression *expression_var(mem_context *mc, GHashTable *table, const char *name);

expression *expression_div(mem_context *mc, expression *e1, expression *e2);
expression *expression_mult(mem_context *mc, expression *e1, expression *e2);
expression *expression_sub(mem_context *mc, expression *e1, expression *e2);
expression *expression_add(mem_context *mc, expression *e1, expression *e2);

expression *expression_parse(mem_context *mc, const char *exp);

float expression_execute(expression_context *context, expression *exp);

#endif /* __EXPRESSION_H__ */
