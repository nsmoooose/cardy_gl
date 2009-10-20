#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression.h"
#include "types.h"

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

void expression_free_token(expression_token *token) {
	free(token);
}

expression_token *expression_create_token(expression_token_type type, const char *str, int length) {
	expression_token *token = 0;
	if(length >= 100) {
		return 0;
	}

	token = calloc(1, sizeof(expression_token));
	token->type = type;
	strncpy(token->content, str, length);
	token->content[length] = 0;
	return token;
}

expression_token** expression_tokenize(const char *exp) {
	expression_token *tokens[1000], **tokens_to_return;
	int token=0, i,j, len = strlen(exp);
	char c;

	memset(tokens, 0, 1000 * sizeof(expression_token*));

	if(len == 0) {
		return 0;
	}

	/* Special considerations:
	 * Remove all whitespace.
	 * Count number of paranthesis and mismatching of them.
	 */

	for(i=0;i<len;++i) {
		c = exp[i];
		if(!(
			   c == '*' || c == '/' ||
			   c == '-' || c == '+' ||
			   (c >= '0' && c <= '9') || c == '.' ||
			   (c >= 'a' && c <= 'z') ||
			   (c >= 'A' && c <= 'Z') ||
			   c == '_')
			)
		{
			return 0;
		}
	}

	for(i=0;i<len;) {
		if(token >= 1000) {
			fprintf(stderr, "Too many tokens in expression.\n");
			goto error;
		}

		c = exp[i];
		if(c == '*' || c == '/' || c == '-' || c == '+') {
			switch(c) {
			case '*':
				tokens[token] = expression_create_token(e_type_op|e_type_mul, 0, 0);
				break;
			case '/':
				tokens[token] = expression_create_token(e_type_op|e_type_div, 0, 0);
				break;
			case '+':
				tokens[token] = expression_create_token(e_type_op|e_type_add, 0, 0);
				break;
			case '-':
				tokens[token] = expression_create_token(e_type_op|e_type_sub, 0, 0);
				break;
			}
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
			tokens[token] = expression_create_token(e_type_const, &exp[i], j-i);
			token++;
			i += (j-i);
		}
		else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
			for(j=i+1;j<len;++j) {
				c = exp[j];
				if(!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')) {
					break;
				}
			}
			tokens[token] = expression_create_token(e_type_var, &exp[i], j-i);
			token++;
			i += (j-i);
		}
		else {
			goto error;
		}
	}

	tokens_to_return = calloc(token+1, sizeof(expression_token*));
	memcpy(tokens_to_return, tokens, sizeof(expression_token*) * token);
	return tokens_to_return;

error:
	token = 0;
	for(token=0;tokens[token];++token) {
		expression_free_token(tokens[token]);
	}
	return 0;
}

int expression_token_count(expression_token *tokens[]) {
	int count = 0;
	for(;;) {
		if(tokens[count]) {
			count++;
		}
		else {
			return count;
		}
	}
}

void expression_free_tokens(expression_token *tokens[]) {
	int index, count = expression_token_count(tokens);
	for(index=0;index<count;++index) {
		expression_free_token(tokens[index]);
	}
	free(tokens);
}

expression* expression_parse_tokens(expression_token *tokens[]) {
	return 0;
}

expression *token_parser(expression_token *tokens[], int current, expression *lhs_in) {
	expression *lhs=0, *rhs=0, *op=0;
	int token_count = expression_token_count(tokens);
	bool look_further=true;

	/* Left hand side. */
	if(lhs_in) {
		lhs = lhs_in;
	}
	else {
		if(tokens[current]->type == e_type_const) {
			lhs = expression_const(atof(tokens[current]->content));
		}
		else if(tokens[current]->type == e_type_var) {
			lhs = expression_var(tokens[current]->content);
		}
		else {
			fprintf(stderr, "Unexpected token (type): %d\n", tokens[current]->type);
			return 0;
		}
		if(current == token_count - 1) {
			return lhs;
		}
		current++;
	}

	/* Check if next operation has priority over this one. */
	if((tokens[current]->type & e_type_add || tokens[current]->type & e_type_sub) &&
	   current + 2 < token_count &&
	   (tokens[current+2]->type & e_type_mul || tokens[current+2]->type & e_type_div)) {

		look_further = false;
		rhs = token_parser(tokens, current + 1, 0);
		if(!rhs) {
			expression_free(lhs);
			return 0;
		}
	}
	else {
		current++;
		if(current >= token_count) {
			fprintf(stderr, "No RHS token.");
			expression_free(lhs);
			return 0;
		}
		if(tokens[current]->type == e_type_const) {
			rhs = expression_const(atof(tokens[current]->content));
		}
		else if(tokens[current]->type == e_type_var) {
			rhs = expression_var(tokens[current]->content);
		}
		else {
			fprintf(stderr, "Unexpected operation (type): %d\n", tokens[current]->type);
			expression_free(lhs);
			return 0;
		}
		current--;
	}

	/* Operation */
	if(tokens[current]->type & e_type_add) {
		op = expression_add(lhs, rhs);
	}
	else if(tokens[current]->type & e_type_sub) {
		op = expression_sub(lhs, rhs);
	}
	else if(tokens[current]->type & e_type_mul) {
		op = expression_mult(lhs, rhs);
	}
	else if(tokens[current]->type & e_type_div) {
		op = expression_div(lhs, rhs);
	}
	else {
		fprintf(stderr, "Unexpected token (type): %d\n", tokens[current]->type);
		expression_free(lhs);
		expression_free(rhs);
		return 0;
	}

	/* Set current to the next operation */
	if(look_further) {
		current+=2;
		if(current < token_count ) {
			/* The above expression has priority. It is the lhs of the next expression. */
			expression *sub = token_parser(tokens, current, op);
			if(!sub) {
				expression_free(op);
			}
			op = sub;
		}
	}
	return op;
}

expression *expression_parse(const char *exp) {
	expression *e = 0;
	expression_token **tokens = expression_tokenize(exp);
	if(tokens) {
		e = token_parser(tokens, 0, 0);
		expression_free_tokens(tokens);
	}
	return e;
}

/* ----------------------------------------------------------------------- */

float expression_execute(expression_context *context, expression *exp) {
	return exp->execute(context, exp);
}
