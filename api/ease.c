#include <math.h>
#include <stdlib.h>
#include "ease.h"

float ease_time_protect4f(ease_function4f function, float t, float b, float c,
                          float d) {
	if (t <= 0.0f)
		return b;
	if (t >= d)
		return c;
	return function(t, b, c, d);
}

float ease_time_protect5f(ease_function5f function, float t, float b, float c,
                          float d, float x1) {
	if (t <= 0.0f)
		return b;
	if (t >= d)
		return c;
	return function(t, b, c, d, x1);
}

float ease_time_protect6f(ease_function6f function, float t, float b, float c,
                          float d, float x1, float x2) {
	if (t <= 0.0f)
		return b;
	if (t >= d)
		return c;
	return function(t, b, c, d, x1, x2);
}

float ease_quad_in(float t, float b, float c, float d) {
	t /= d;
	return c * t * t + b;
}

float ease_quad_out(float t, float b, float c, float d) {
	t /= d;
	return -c * t * (t - 2) + b;
}

#if 0
float ease_quad_in_out(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t + b;
	return -c/2 * ((--t)*(t-2) - 1) + b;
}

float ease_cubic_in(float t, float b, float c, float d) {
	return c*(t/=d)*t*t + b;
}

float ease_cubic_out(float t, float b, float c, float d) {
	return c*((t=t/d-1)*t*t + 1) + b;
}

float ease_cubic_in_out(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t + b;
	return c/2*((t-=2)*t*t + 2) + b;
}

float ease_quart_in(float t, float b, float c, float d) {
	return c*(t/=d)*t*t*t + b;
}

float ease_quart_out(float t, float b, float c, float d) {
	return -c * ((t=t/d-1)*t*t*t - 1) + b;
}

float ease_quart_in_out(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
	return -c/2 * ((t-=2)*t*t*t - 2) + b;
}

float ease_quint_in(float t, float b, float c, float d) {
	return c*(t/=d)*t*t*t*t + b;
}
#endif

float ease_quint_out(float t, float b, float c, float d) {
	t = t / d - 1;
	return c * ((t)*t * t * t * t + 1) + b;
}

#if 0
float ease_quint_in_out(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
	return c/2*((t-=2)*t*t*t*t + 2) + b;
}

float ease_sine_in(float t, float b, float c, float d) {
	return -c * cos(t/d * (M_PI/2)) + c + b;
}

float ease_sine_out(float t, float b, float c, float d) {
	return c * sin(t/d * (M_PI/2)) + b;
}

float ease_sine_in_out(float t, float b, float c, float d) {
	return -c/2 * (cos(M_PI*t/d) - 1) + b;
}

float ease_expo_in(float t, float b, float c, float d) {
	return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
}

float ease_expo_out(float t, float b, float c, float d) {
	return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;
}

float ease_expo_in_out(float t, float b, float c, float d) {
	if (t==0) return b;
	if (t==d) return b+c;
	if ((t/=d/2) < 1) return c/2 * pow(2, 10 * (t - 1)) + b;
	return c/2 * (-pow(2, -10 * --t) + 2) + b;
}

float ease_bounce_in(float t, float b, float c, float d) {
	return c - ease_bounce_out(d-t, 0, c, d) + b;
}

float ease_bounce_out(float t, float b, float c, float d) {
	if ((t/=d) < (1/2.75)) {
		return c*(7.5625*t*t) + b;
	} else if (t < (2/2.75)) {
		return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
	} else if (t < (2.5/2.75)) {
		return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
	} else {
		return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
	}
}

float ease_bounce_in_out(float t, float b, float c, float d) {
	if (t < d/2) return ease_bounce_in(t*2, 0, c, d) * .5 + b;
	else return ease_bounce_out(t*2-d, 0, c, d) * .5 + c*.5 + b;
}

float ease_back_in(float t, float b, float c, float d, float s) {
	/* if (s == undefined) s = 1.70158; */
	return c*(t/=d)*t*((s+1)*t - s) + b;
}

float ease_back_out(float t, float b, float c, float d, float s) {
	/* if (s == undefined) s = 1.70158; */
	return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

float ease_back_in_out(float t, float b, float c, float d, float s) {
	/* if (s == undefined) s = 1.70158; */
	if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
	return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
}

float ease_circ_in(float t, float b, float c, float d) {
	return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
}

float ease_circ_out(float t, float b, float c, float d) {
	return c * sqrt(1 - (t=t/d-1)*t) + b;
}

float ease_circ_in_out(float t, float b, float c, float d) {
	if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
	return c/2 * (sqrt(1 - (t-=2)*t) + 1) + b;
}

float ease_elastic_in(float t, float b, float c, float d, float a, float p) {
	float s;
	if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
	if (!a || a < abs(c)) { a=c; s=p/4; }
	else s = p/(2*M_PI) * asin (c/a);
	return -(a*pow(2,10*(t-=1)) * sin( (t*d-s)*(2*M_PI)/p )) + b;
}

float ease_elastic_out(float t, float b, float c, float d, float a, float p) {
	float s;
	if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
	if (!a || a < abs(c)) { a=c; s=p/4; }
	else s = p/(2*M_PI) * asin (c/a);
	return (a*pow(2,-10*t) * sin( (t*d-s)*(2*M_PI)/p ) + c + b);
}

float ease_elastic_in_out(float t, float b, float c, float d, float a, float p) {
	float s;
	if (t==0) return b;  if ((t/=d/2)==2) return b+c;  if (!p) p=d*(.3*1.5);
	if (!a || a < abs(c)) { a=c; s=p/4; }
	else s = p/(2*M_PI) * asin (c/a);
	if (t < 1) return -.5*(a*pow(2,10*(t-=1)) * sin( (t*d-s)*(2*M_PI)/p )) + b;
	return a*pow(2,-10*(t-=1)) * sin( (t*d-s)*(2*M_PI)/p )*.5 + c + b;
}
#endif

float ease_linear(float t, float b, float c, float d) { return c * t / d + b; }
