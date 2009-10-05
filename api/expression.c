#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression.h"

struct expression_context_St {
	GHashTable *expressions;
};

struct expression_St {
	void *data;
	float (*execute)(expression_context *context, expression *exp);
};

expression_context *expression_context_create() {
	expression_context *ec = calloc(1, sizeof(expression_context));
	ec->expressions = g_hash_table_new(g_str_hash, g_str_equal);
	return ec;
}

void expression_context_free(expression_context *ec) {
	/* TODO
	   Iterate through all expressions and free them.
	*/
	g_hash_table_unref(ec->expressions);
}

void expression_context_set(
	expression_context *ec, const char *key, expression *value) {
	expression *old = expression_context_get(ec, key);
	if(old) {
		expression_free(old);
	}

	g_hash_table_insert(ec->expressions, (char*)key, value);
}

expression *expression_context_get(expression_context *ec, const char *key) {
	return g_hash_table_lookup(ec->expressions, key);
}

/* ----------------------------------------------------------------------- */

void expression_free(expression *e) {
	/* TODO
	   This expression can have subexpressions that also needs freeing. */

	if(e->data) {
		free(e->data);
	}
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
	if(var) {
		return expression_execute(ec, var);
	}

	/* TODO
	   Better error handling when there are expression errors.
	**/
	fprintf(stderr, "Invalid expression. Variable: %s not found.", (char*)e->data);
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

expression *token_parser(char *tokens[], int current) {
	char c = tokens[current][0];
	expression *lhs=0, *rhs=0;

	/* Left hand side. */
	if(c>='0' && c <= '9') {
		lhs = expression_const(atof(tokens[current]));
		if(tokens[current+1] == 0) {
			return lhs;
		}
	}
	else {
		fprintf(stderr, "Unexpected token: %s\n", tokens[current]);
		return 0;
	}

	/* Right hand side */
	current += 2;
	if(tokens[current] == 0) {
		fprintf(stderr, "No RHS token.");
		exit(1);
	}
	c = tokens[current][0];
	if(c>='0' && c <= '9') {
		rhs = expression_const(atof(tokens[current]));
	}
	else {
		fprintf(stderr, "Unexpected operation: %c\n", c);
		expression_free(lhs);
		return 0;
	}

	/* Operation */
	current--;
	c = tokens[current][0];
	if(c == '+') {
		return expression_add(lhs, rhs);
	}
	else if(c == '-') {
		return expression_sub(lhs, rhs);
	}
	else if(c == '*') {
		return expression_mult(lhs, rhs);
	}
	else if(c == '/') {
		return expression_div(lhs, rhs);
	}
	else {
		fprintf(stderr, "Unexpected token: %s\n", tokens[current]);
		expression_free(lhs);
		expression_free(rhs);
		return 0;
	}
}

expression *expression_parse(const char *exp) {
	char *tokens[1000];
	int token=0, i,j, len = strlen(exp);
	char c;
	expression *e = 0;

	memset(tokens, 0, 1000 * sizeof(char*));

	printf("--------------------\nParsing: %s\n", exp);

	if(len == 0) {
		return 0;
	}

	/* Special considerations:
	 * Remove all whitespace.
	 * Count number of paranthesis and mismatching of them.
	 */

	for(i=0;i<len;++i) {
		c = exp[i];
		if(!(c == '*' || c == '/' || c == '-' || c == '+' ||
		   ((c >= '0' && c <= '9') || c == '.') ||
			((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))) {
			return 0;
		}
	}

	for(i=0;i<len;) {
		if(token >= 1000) {
			fprintf(stderr, "Too many tokens in expression.\n");
			goto cleanup;
		}

		c = exp[i];
		if(c == '*' || c == '/' || c == '-' || c == '+') {
			printf("Found token: %c\n", c);
			tokens[token] = calloc(2, sizeof(char));
			tokens[token][0] = c;
			tokens[token][1] = 0;
			token++;
			i++;
		}
		else if((c >= '0' && c <= '9') || c == '.') {
			for(j=i+1;j<len;++j) {
				c = exp[j];
				if(!((c >= '0' && c <= '9') || c == '.')) {
					break;
				}
			}
			tokens[token] = calloc(1, j-i + 1);
			strncpy(tokens[token], &exp[i], j-i);
			printf("Found token: %s\n", tokens[token]);
			token++;
			i += (j-i);
		}
		else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		}
	}

	for(i=0;i<1000;++i) {
		if(tokens[i] == 0)
			break;
		printf("Token: %s\n", tokens[i]);
	}

	e = token_parser(tokens, 0);

cleanup:
	for(i=0;i<token;++i) {
		free(tokens[i]);
	}
	return e;
}

/* ----------------------------------------------------------------------- */

float expression_execute(expression_context *context, expression *exp) {
	return exp->execute(context, exp);
}
