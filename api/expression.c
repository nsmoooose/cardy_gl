#include <glib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api/expression.h"

struct expression_context_St {
	GHashTable *expressions;
};

struct expression_St {
	void *data;
	float (*execute)(expression_context *context, expression *exp);
};

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

expression_context *expression_context_create(void) {
	expression_context *ec = calloc(1, sizeof(expression_context));
	ec->expressions = g_hash_table_new(g_str_hash, g_str_equal);
	return ec;
}

void expression_context_free(expression_context *ec) {
	/* TODO: Iterate through all expressions and free them.	*/
	g_hash_table_unref(ec->expressions);
}

void expression_context_set(expression_context *ec, const char *key,
                            expression *value) {
	expression *old = expression_context_get(ec, key);
	if (old) {
		expression_free(old);
	}

	g_hash_table_insert(ec->expressions, (char *)key, value);
}

expression *expression_context_get(expression_context *ec, const char *key) {
	return g_hash_table_lookup(ec->expressions, key);
}

/* ----------------------------------------------------------------------- */

void expression_free(expression *e) {
	/* TODO: This expression can have subexpressions that also needs freeing. */
	free(e->data);
	free(e);
}

/* ----------------------------------------------------------------------- */

typedef struct {
	float value;
} expression_const_data;

static float expression_const_execute(expression_context *ec, expression *e) {
	expression_const_data *d = e->data;
	return d->value;
}

expression *expression_const(float value) {
	expression *e = calloc(1, sizeof(expression));
	expression_const_data *d = calloc(1, sizeof(expression_const_data));
	d->value = value;
	e->data = d;
	e->execute = expression_const_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_var_execute(expression_context *ec, expression *e) {
	expression *var = expression_context_get(ec, e->data);
	if (var) {
		return expression_execute(ec, var);
	}

	/* TODO: Better error handling when there are expression errors. */
	fprintf(stderr, "Invalid expression. Variable: %s not found.",
	        (char *)e->data);
	exit(1);
}

expression *expression_var(const char *name) {
	expression *e = calloc(1, sizeof(expression));
	e->data = strdup(name);
	e->execute = expression_var_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

typedef struct {
	float *pointer;
} expression_pointer_data;

static float expression_pointer_execute(expression_context *ec, expression *e) {
	expression_pointer_data *d = e->data;
	return *d->pointer;
}

expression *expression_pointer(float *var) {
	expression *e = calloc(1, sizeof(expression));
	expression_pointer_data *d = calloc(1, sizeof(expression_pointer_data));
	d->pointer = var;
	e->data = d;
	e->execute = expression_pointer_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

typedef struct {
	union {
		function1f f1;
		function2f f2;
		function3f f3;
		function4f f4;
		function5f f5;
		function6f f6;
	} function;
	int param_count;
	expression *params[6];
} expression_function_data;

static float expression_function_execute(expression_context *ec,
                                         expression *e) {
	expression_function_data *d = e->data;
	switch (d->param_count) {
	case 1:
		return d->function.f1(expression_execute(ec, d->params[0]));
	case 2:
		return d->function.f2(expression_execute(ec, d->params[0]),
		                      expression_execute(ec, d->params[1]));
	case 3:
		return d->function.f3(expression_execute(ec, d->params[0]),
		                      expression_execute(ec, d->params[1]),
		                      expression_execute(ec, d->params[2]));
	case 4:
		return d->function.f4(expression_execute(ec, d->params[0]),
		                      expression_execute(ec, d->params[1]),
		                      expression_execute(ec, d->params[2]),
		                      expression_execute(ec, d->params[3]));
	case 5:
		return d->function.f5(expression_execute(ec, d->params[0]),
		                      expression_execute(ec, d->params[1]),
		                      expression_execute(ec, d->params[2]),
		                      expression_execute(ec, d->params[3]),
		                      expression_execute(ec, d->params[4]));
	case 6:
		return d->function.f6(expression_execute(ec, d->params[0]),
		                      expression_execute(ec, d->params[1]),
		                      expression_execute(ec, d->params[2]),
		                      expression_execute(ec, d->params[3]),
		                      expression_execute(ec, d->params[4]),
		                      expression_execute(ec, d->params[5]));
	default:
		fprintf(stderr, "Invalid number of parameters to expression.\n");
		exit(1);
	}
}

expression *expression_function1f(function1f function, expression *e1) {
	expression *e = calloc(1, sizeof(expression));
	expression_function_data *d = calloc(1, sizeof(expression_function_data));
	d->function.f1 = function;
	d->param_count = 1;
	d->params[0] = e1;
	e->data = d;
	e->execute = expression_function_execute;
	return e;
}

expression *expression_function2f(function2f function, expression *params[]) {
	expression *e = calloc(1, sizeof(expression));
	expression_function_data *d = calloc(1, sizeof(expression_function_data));
	d->function.f2 = function;
	d->param_count = 2;
	d->params[0] = params[0];
	d->params[1] = params[1];
	e->data = d;
	e->execute = expression_function_execute;
	return e;
}

expression *expression_function3f(function3f function, expression *params[]) {
	expression *e = calloc(1, sizeof(expression));
	expression_function_data *d = calloc(1, sizeof(expression_function_data));
	d->function.f3 = function;
	d->param_count = 3;
	d->params[0] = params[0];
	d->params[1] = params[1];
	d->params[2] = params[2];
	e->data = d;
	e->execute = expression_function_execute;
	return e;
}

expression *expression_function4f(function4f function, expression *params[]) {
	expression *e = calloc(1, sizeof(expression));
	expression_function_data *d = calloc(1, sizeof(expression_function_data));
	d->function.f4 = function;
	d->param_count = 4;
	d->params[0] = params[0];
	d->params[1] = params[1];
	d->params[2] = params[2];
	d->params[3] = params[3];
	e->data = d;
	e->execute = expression_function_execute;
	return e;
}

expression *expression_function5f(function5f function, expression *params[]) {
	expression *e = calloc(1, sizeof(expression));
	expression_function_data *d = calloc(1, sizeof(expression_function_data));
	d->function.f5 = function;
	d->param_count = 5;
	d->params[0] = params[0];
	d->params[1] = params[1];
	d->params[2] = params[2];
	d->params[3] = params[3];
	d->params[4] = params[4];
	e->data = d;
	e->execute = expression_function_execute;
	return e;
}

expression *expression_function6f(function6f function, expression *params[]) {
	expression *e = calloc(1, sizeof(expression));
	expression_function_data *d = calloc(1, sizeof(expression_function_data));
	d->function.f6 = function;
	d->param_count = 6;
	d->params[0] = params[0];
	d->params[1] = params[1];
	d->params[2] = params[2];
	d->params[3] = params[3];
	d->params[4] = params[4];
	d->params[5] = params[5];
	e->data = d;
	e->execute = expression_function_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

typedef struct {
	expression *e1, *e2;
} expression_op_data;

expression *expression_op(expression *e1, expression *e2) {
	expression *e = calloc(1, sizeof(expression));
	expression_op_data *d = calloc(1, sizeof(expression_op_data));
	d->e1 = e1;
	d->e2 = e2;
	e->data = d;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_div_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) / expression_execute(ec, d->e2);
}

expression *expression_div(expression *e1, expression *e2) {
	expression *e = expression_op(e1, e2);
	e->execute = expression_div_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_mult_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) * expression_execute(ec, d->e2);
}

expression *expression_mult(expression *e1, expression *e2) {
	expression *e = expression_op(e1, e2);
	e->execute = expression_mult_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_sub_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) - expression_execute(ec, d->e2);
}

expression *expression_sub(expression *e1, expression *e2) {
	expression *e = expression_op(e1, e2);
	e->execute = expression_sub_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_add_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) + expression_execute(ec, d->e2);
}

expression *expression_add(expression *e1, expression *e2) {
	expression *e = expression_op(e1, e2);
	e->execute = expression_add_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

void expression_free_token(expression_token *token) { free(token); }

expression_token *expression_create_token(expression_token_type type,
                                          const char *str, int length) {
	expression_token *token = 0;
	if (length >= 100) {
		return 0;
	}

	token = calloc(1, sizeof(expression_token));
	token->type = type;
	strncpy(token->content, str, length);
	token->content[length] = 0;
	return token;
}

expression_token **expression_tokenize(const char *exp) {
	expression_token *tokens[1000], **tokens_to_return;
	int token = 0, i, j, len = strlen(exp);
	char c;
	bool op_mode = false;

	memset(tokens, 0, 1000 * sizeof(expression_token *));

	if (len == 0) {
		return 0;
	}

	/* Special considerations:
	 * Remove all whitespace.
	 * Count number of paranthesis and mismatching of them.
	 */

	for (i = 0; i < len; ++i) {
		c = exp[i];
		if (!(c == '(' || c == ')' || c == '*' || c == '/' || c == '-' ||
		      c == '+' || (c >= '0' && c <= '9') || c == '.' ||
		      (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')) {
			return 0;
		}
	}

	for (i = 0; i < len;) {
		if (token >= 1000) {
			fprintf(stderr, "Too many tokens in expression.\n");
			goto error;
		}

		c = exp[i];

		if (c == '(') {
			if (token > 0 && tokens[token - 1]->type == e_type_var) {
				tokens[token - 1]->type = e_type_function;
				op_mode = false;
			}

			tokens[token] = expression_create_token(e_type_leftp, 0, 0);
			token++;
			i++;
		} else if (c == ')') {
			tokens[token] = expression_create_token(e_type_rightp, 0, 0);
			token++;
			i++;
		} else {
			/* Operation mode means that we are looking for a multiplication,
			   subtraction, addition or division. */
			if (op_mode) {
				if (c == '*' || c == '/' || c == '-' || c == '+') {
					switch (c) {
					case '*':
						tokens[token] = expression_create_token(
							e_type_op | e_type_mul, 0, 0);
						break;
					case '/':
						tokens[token] = expression_create_token(
							e_type_op | e_type_div, 0, 0);
						break;
					case '+':
						tokens[token] = expression_create_token(
							e_type_op | e_type_add, 0, 0);
						break;
					case '-':
						tokens[token] = expression_create_token(
							e_type_op | e_type_sub, 0, 0);
						break;
					}
					token++;
					i++;
				} else {
					goto error;
				}
				op_mode = false;
			} else {
				/* No we are looking for a variable or a constant value. */
				if ((c >= '0' && c <= '9') || c == '.' || c == '-' ||
				    c == '+') {
					for (j = i + 1; j < len; ++j) {
						c = exp[j];
						if (!((c >= '0' && c <= '9') || c == '.')) {
							break;
						}
					}
					tokens[token] =
						expression_create_token(e_type_const, &exp[i], j - i);
					token++;
					i += (j - i);
				} else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
				           c == '_') {
					for (j = i + 1; j < len; ++j) {
						c = exp[j];
						if (!((c >= 'a' && c <= 'z') ||
						      (c >= 'A' && c <= 'Z') || c == '_')) {
							break;
						}
					}
					tokens[token] =
						expression_create_token(e_type_var, &exp[i], j - i);
					token++;
					i += (j - i);
				} else {
					goto error;
				}
				op_mode = true;
			}
		}
	}

	tokens_to_return = calloc(token + 1, sizeof(expression_token *));
	memcpy(tokens_to_return, tokens, sizeof(expression_token *) * token);
	return tokens_to_return;

error:
	for (token = 0; tokens[token]; ++token) {
		expression_free_token(tokens[token]);
	}
	return 0;
}

int expression_token_count(expression_token *tokens[]) {
	int count = 0;
	for (;;) {
		if (tokens[count]) {
			count++;
		} else {
			return count;
		}
	}
}

void expression_free_tokens(expression_token *tokens[]) {
	int index, count = expression_token_count(tokens);
	for (index = 0; index < count; ++index) {
		expression_free_token(tokens[index]);
	}
	free(tokens);
}

static int find_matching_paranthesis(expression_token *tokens[], int current) {
	int token_count = expression_token_count(tokens);
	int index = 0, count = 0;
	for (; current + index < token_count; ++index) {
		if (tokens[current + index]->type == e_type_leftp) {
			count++;
		} else if (tokens[current + index]->type == e_type_rightp) {
			count--;
		}
		if (count == 0) {
			return current + index;
		}
	}
	return -1;
}

static expression *expression_parse_tokens_imp(expression_token *tokens[],
                                               int current,
                                               expression *lhs_in) {
	expression *lhs = 0, *rhs = 0, *op = 0;
	int token_count = expression_token_count(tokens);
	int next = 0;
	bool look_further = true;

	/* Left hand side. */
	if (lhs_in) {
		lhs = lhs_in;
	} else {
		if (tokens[current]->type == e_type_leftp) {
			lhs = expression_parse_tokens_imp(tokens, current + 1, 0);
			current = find_matching_paranthesis(tokens, current);
		} else if (tokens[current]->type == e_type_const) {
			lhs = expression_const(atof(tokens[current]->content));
		} else if (tokens[current]->type == e_type_var) {
			lhs = expression_var(tokens[current]->content);
		} else {
			fprintf(stderr, "Unexpected token lhs (type): %d\n",
			        tokens[current]->type);
			return 0;
		}
		current++;
	}

	if (current > token_count - 1) {
		/* No more tokens. Lets return what we have. */
		return lhs;
	}

	if (tokens[current]->type == e_type_rightp) {
		return lhs;
	}

	if ((current + 1) > (token_count - 1)) {
		/* We got an operation to perform but no expression to the right. */
		expression_free(lhs);
		return 0;
	}

	/* Check if next operation has priority over this one. */
	if ((tokens[current]->type & e_type_add ||
	     tokens[current]->type & e_type_sub) &&
	    current + 2 < token_count &&
	    (tokens[current + 2]->type & e_type_mul ||
	     tokens[current + 2]->type & e_type_div)) {

		look_further = false;
		rhs = expression_parse_tokens_imp(tokens, current + 1, 0);
		if (!rhs) {
			expression_free(lhs);
			return 0;
		}
	} else {
		current++;
		if (current >= token_count) {
			fprintf(stderr, "No RHS token.");
			expression_free(lhs);
			return 0;
		}
		if (tokens[current]->type == e_type_leftp) {
			rhs = expression_parse_tokens_imp(tokens, current + 1, 0);
			/* Set current to the next operation. We must then match
			   the next paranthesis. */
			next = find_matching_paranthesis(tokens, current) + 1;
		} else if (tokens[current]->type == e_type_const) {
			rhs = expression_const(atof(tokens[current]->content));
			next = current + 1;
		} else if (tokens[current]->type == e_type_var) {
			rhs = expression_var(tokens[current]->content);
			next = current + 1;
		} else {
			fprintf(stderr, "Unexpected operation (type): %d\n",
			        tokens[current]->type);
			expression_free(lhs);
			return 0;
		}
		current--;
	}

	/* Operation */
	if (tokens[current]->type & e_type_add) {
		op = expression_add(lhs, rhs);
	} else if (tokens[current]->type & e_type_sub) {
		op = expression_sub(lhs, rhs);
	} else if (tokens[current]->type & e_type_mul) {
		op = expression_mult(lhs, rhs);
	} else if (tokens[current]->type & e_type_div) {
		op = expression_div(lhs, rhs);
	} else {
		fprintf(stderr, "Unexpected token rhs (type): %d\n",
		        tokens[current]->type);
		expression_free(lhs);
		expression_free(rhs);
		return 0;
	}

	/* Set current to the next operation */
	if (look_further) {
		current = next;
		if (current < token_count && tokens[current]->type == e_type_rightp) {
			return op;
		}
		if (current < token_count) {
			/* The above expression has priority. It is the lhs of the next
			 * expression. */
			expression *sub = expression_parse_tokens_imp(tokens, current, op);
			if (!sub) {
				expression_free(op);
			}
			op = sub;
		}
	}
	return op;
}

expression *expression_parse_tokens(expression_token *tokens[]) {
	return expression_parse_tokens_imp(tokens, 0, 0);
}

expression *expression_parse(expression_lib *library, const char *exp) {
	expression *e = 0;
	expression_token **tokens = expression_tokenize(exp);
	if (tokens) {
		e = expression_parse_tokens(tokens);
		expression_free_tokens(tokens);
	}
	return e;
}

/* ----------------------------------------------------------------------- */

float expression_execute(expression_context *context, expression *exp) {
	return exp->execute(context, exp);
}

expression_lib *expression_lib_create(void) {
	expression_lib *lib = calloc(1, sizeof(expression_lib));
	lib->functions = g_hash_table_new(g_str_hash, g_str_equal);
	return lib;
}

void expression_lib_free(expression_lib *lib) {
	/* TODO: Iterate through all functions and free them. */
	g_hash_table_unref(lib->functions);
	free(lib);
}

static void expression_lib_add_math_h(expression_lib *lib) {
	expression_lib_add_function1f(lib, "cos", cosf);
	expression_lib_add_function1f(lib, "sin", sinf);
	expression_lib_add_function1f(lib, "acos", acosf);
	expression_lib_add_function1f(lib, "asin", asinf);
}

static void expression_lib_add_ease_h(expression_lib *lib) {}

expression_lib *expression_lib_default(void) {
	expression_lib *lib = expression_lib_create();
	expression_lib_add_math_h(lib);
	expression_lib_add_ease_h(lib);
	return lib;
}

void expression_lib_add_function1f(expression_lib *lib, const char *name,
                                   function1f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 1;
	f->function.f1 = fun;
	g_hash_table_insert(lib->functions, (char *)name, f);
}

void expression_lib_add_function2f(expression_lib *lib, const char *name,
                                   function2f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 2;
	f->function.f2 = fun;
	g_hash_table_insert(lib->functions, (char *)name, f);
}

void expression_lib_add_function3f(expression_lib *lib, const char *name,
                                   function3f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 3;
	f->function.f3 = fun;
	g_hash_table_insert(lib->functions, (char *)name, f);
}

void expression_lib_add_function4f(expression_lib *lib, const char *name,
                                   function4f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 4;
	f->function.f4 = fun;
	g_hash_table_insert(lib->functions, (char *)name, f);
}

void expression_lib_add_function5f(expression_lib *lib, const char *name,
                                   function5f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 5;
	f->function.f5 = fun;
	g_hash_table_insert(lib->functions, (char *)name, f);
}

void expression_lib_add_function6f(expression_lib *lib, const char *name,
                                   function6f fun) {
	expression_lib_function *f = calloc(1, sizeof(expression_lib_function));
	f->param_count = 6;
	f->function.f6 = fun;
	g_hash_table_insert(lib->functions, (char *)name, f);
}

bool expression_lib_is_function(expression_lib *lib, const char *name) {
	return g_hash_table_lookup(lib->functions, name) != 0;
}

int expression_lib_param_count(expression_lib *lib, const char *name) {
	expression_lib_function *f = g_hash_table_lookup(lib->functions, name);
	return f->param_count;
}

expression *expression_lib_build_expression(expression_lib *lib,
                                            const char *name,
                                            expression *params[]) {
	expression_lib_function *f = g_hash_table_lookup(lib->functions, name);
	switch (f->param_count) {
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
