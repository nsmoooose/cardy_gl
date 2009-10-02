#include "expression.h"

struct expression_context_St {
	int dummy;
};

struct expression_St {
	void *data;
	float (*execute)(expression_context *context, expression *exp);
};

expression_context *expression_context_create(mem_context *mc) {
	return 0;
}

/* -------------------------------------------------------------------------- */

typedef struct {
	float value;
} expression_const_data;

static float expression_const_execute(expression_context *ec, expression *e) {
	expression_const_data *d = e->data;
	return d->value;
}

expression *expression_const(mem_context *mc, float value) {
	expression *e = mem_alloc(mc, sizeof(expression));
	expression_const_data *d = mem_alloc(mc, sizeof(expression_const_data));
	d->value = value;
	e->data = d;
	e->execute = expression_const_execute;
	return e;
}

/* -------------------------------------------------------------------------- */

expression *expression_var(mem_context *mc, const char *name) {
	return 0;
}

expression *expression_div(mem_context *mc, expression *e1, expression *e2) {
	return 0;
}

expression *expression_mult(mem_context *mc, expression *e1, expression *e2) {
	return 0;
}

expression *expression_sub(mem_context *mc, expression *e1, expression *e2) {
	return 0;
}

expression *expression_add(mem_context *mc, expression *e1, expression *e2) {
	return 0;
}

expression *expression_parse(mem_context *mc, const char *exp) {
	return 0;
}

float expression_execute(expression_context *context, expression *exp) {
	return exp->execute(context, exp);
}
