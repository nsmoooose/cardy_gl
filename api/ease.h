#ifndef __EASING_H__
#define __EASING_H__

/*
   Ease between values....
   t = Current time.
   b = From
   c = Change
   d = Total time the animation should take.

   This is a good reference page:
   http://www.robertpenner.com/easing/
 */

typedef float (*ease_function4f)(float t, float b, float c, float d);
typedef float (*ease_function5f)(float t, float b, float c, float d, float x1);
typedef float (*ease_function6f)(float t, float b, float c, float d, float x1, float x2);

/* Protect the easing algorithm to generate incorrect output when
   time is < 0.0 or larger than d. */
float ease_time_protect4f(ease_function4f function, float t, float b, float c, float d);
float ease_time_protect5f(ease_function5f function, float t, float b, float c, float d, float x1);
float ease_time_protect6f(ease_function6f function, float t, float b, float c, float d, float x1, float x2);

float ease_quad_in(float t, float b, float c, float d);
float ease_quad_out(float t, float b, float c, float d);
float ease_quad_in_out(float t, float b, float c, float d);

float ease_cubic_in(float t, float b, float c, float d);
float ease_cubic_out(float t, float b, float c, float d);
float ease_cubic_in_out(float t, float b, float c, float d);

float ease_quart_in(float t, float b, float c, float d);
float ease_quart_out(float t, float b, float c, float d);
float ease_quart_in_out(float t, float b, float c, float d);

float ease_quint_in(float t, float b, float c, float d);
float ease_quint_out(float t, float b, float c, float d);
float ease_quint_in_out(float t, float b, float c, float d);

float ease_sine_in(float t, float b, float c, float d);
float ease_sine_out(float t, float b, float c, float d);
float ease_sine_in_out(float t, float b, float c, float d);

float ease_expo_in(float t, float b, float c, float d);
float ease_expo_out(float t, float b, float c, float d);
float ease_expo_in_out(float t, float b, float c, float d);

float ease_bounce_in(float t, float b, float c, float d);
float ease_bounce_out(float t, float b, float c, float d);
float ease_bounce_in_out(float t, float b, float c, float d);

float ease_back_in(float t, float b, float c, float d, float s);
float ease_back_out(float t, float b, float c, float d, float s);
float ease_back_in_out(float t, float b, float c, float d, float s);

float ease_circ_in(float t, float b, float c, float d);
float ease_circ_out(float t, float b, float c, float d);
float ease_circ_in_out(float t, float b, float c, float d);

float ease_elastic_in(float t, float b, float c, float d, float a, float p);
float ease_elastic_out(float t, float b, float c, float d, float a, float p);
float ease_elastic_in_out(float t, float b, float c, float d, float a, float p);

float ease_linear(float t, float b, float c, float d);

#endif /* __EASING_H__ */
