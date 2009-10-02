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

/* ----------------------------------------------------------------------- */

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

/* ----------------------------------------------------------------------- */

expression *expression_var(
	mem_context *mc, GHashTable *table, const char *name) {
	return 0;
}

/* ----------------------------------------------------------------------- */

typedef struct {
	expression *e1, *e2;
} expression_op_data;

expression *expression_op(mem_context *mc, expression *e1, expression *e2) {
	expression *e = mem_alloc(mc, sizeof(expression));
	expression_op_data *d = mem_alloc(mc, sizeof(expression_op_data));
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

expression *expression_div(mem_context *mc, expression *e1, expression *e2) {
	expression *e = expression_op(mc, e1, e2);
	e->execute = expression_div_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_mult_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) * expression_execute(ec, d->e2);
}

expression *expression_mult(mem_context *mc, expression *e1, expression *e2) {
	expression *e = expression_op(mc, e1, e2);
	e->execute = expression_mult_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_sub_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) - expression_execute(ec, d->e2);
}

expression *expression_sub(mem_context *mc, expression *e1, expression *e2) {
	expression *e = expression_op(mc, e1, e2);
	e->execute = expression_sub_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

static float expression_add_execute(expression_context *ec, expression *e) {
	expression_op_data *d = e->data;
	return expression_execute(ec, d->e1) + expression_execute(ec, d->e2);
}

expression *expression_add(mem_context *mc, expression *e1, expression *e2) {
	expression *e = expression_op(mc, e1, e2);
	e->execute = expression_add_execute;
	return e;
}

/* ----------------------------------------------------------------------- */

expression *expression_parse(mem_context *mc, const char *exp) {
	return 0;
}

/* ----------------------------------------------------------------------- */

float expression_execute(expression_context *context, expression *exp) {
	return exp->execute(context, exp);
}
