#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include <glib.h>
#include "memory.h"

struct expression_context_St;
typedef struct expression_context_St expression_context;

struct expression_St;
typedef struct expression_St expression;

expression_context *expression_context_create(mem_context *mc);
void expression_context_free(mem_context *mc, expression_context *ec);
void expression_context_set(
	expression_context *ec, const char *key, expression *value);
expression *expression_context_get(expression_context *ec, const char *key);

void expression_free(mem_context *mc, expression *e);
expression *expression_const(mem_context *mc, float value);

expression *expression_var(mem_context *mc, const char *name);

expression *expression_div(mem_context *mc, expression *e1, expression *e2);
expression *expression_mult(mem_context *mc, expression *e1, expression *e2);
expression *expression_sub(mem_context *mc, expression *e1, expression *e2);
expression *expression_add(mem_context *mc, expression *e1, expression *e2);

expression *expression_parse(mem_context *mc, const char *exp);

float expression_execute(expression_context *context, expression *exp);

#endif /* __EXPRESSION_H__ */
