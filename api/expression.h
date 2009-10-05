#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include <glib.h>

struct expression_context_St;
typedef struct expression_context_St expression_context;

struct expression_St;
typedef struct expression_St expression;

expression_context *expression_context_create();
void expression_context_free(expression_context *ec);
void expression_context_set(
	expression_context *ec, const char *key, expression *value);
expression *expression_context_get(expression_context *ec, const char *key);

void expression_free(expression *e);
expression *expression_const(float value);

expression *expression_var(const char *name);

expression *expression_div(expression *e1, expression *e2);
expression *expression_mult(expression *e1, expression *e2);
expression *expression_sub(expression *e1, expression *e2);
expression *expression_add(expression *e1, expression *e2);

expression *expression_parse(const char *exp);

float expression_execute(expression_context *context, expression *exp);

#endif /* __EXPRESSION_H__ */
