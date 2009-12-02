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
expression *expression_pointer(float *var);

typedef float (*function1f)(float);
typedef float (*function2f)(float, float);
typedef float (*function3f)(float, float, float);
typedef float (*function4f)(float, float, float, float);
typedef float (*function5f)(float, float, float, float, float);
typedef float (*function6f)(float, float, float, float, float, float);

expression *expression_function1f(function1f function, expression *e1);
expression *expression_function2f(function2f function, expression *e[]);
expression *expression_function3f(function3f function, expression *e[]);
expression *expression_function4f(function4f function, expression *e[]);
expression *expression_function5f(function5f function, expression *e[]);
expression *expression_function6f(function6f function, expression *e[]);

expression *expression_div(expression *e1, expression *e2);
expression *expression_mult(expression *e1, expression *e2);
expression *expression_sub(expression *e1, expression *e2);
expression *expression_add(expression *e1, expression *e2);

typedef enum {
	e_type_op = 1 << 0,
	e_type_div = 1 << 1,
	e_type_mul = 1 << 2,
	e_type_add = 1 << 3,
	e_type_sub = 1 << 4,

	e_type_leftp = 1 << 5,
	e_type_rightp = 1 << 6,

	e_type_var = 1 << 7,
	e_type_const = 1 << 8,
	e_type_function = 1 << 9
} expression_token_type;

typedef struct {
	expression_token_type type;
	char content[100];
} expression_token;

void expression_free_token(expression_token *token);
expression_token *expression_create_token(expression_token_type type, const char *str, int length);
expression_token** expression_tokenize(const char *exp);
void expression_free_tokens(expression_token *tokens[]);
expression* expression_parse_tokens(expression_token *tokens[]);
int expression_token_count(expression_token *tokens[]);

expression *expression_parse(const char *exp);

float expression_execute(expression_context *context, expression *exp);

#endif /* __EXPRESSION_H__ */
