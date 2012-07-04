#pragma once

#include <math.h>

template <int N> inline float faustpower(float x) 		{ return powf(x,N); } 
template <int N> inline double faustpower(double x) 	{ return pow(x,N); }
template <int N> inline int faustpower(int x) 			{ return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x) 		{ return 1; }
template <> 	 inline int faustpower<1>(int x) 		{ return x; }

#ifndef FAUSTFLOAT
#define FAUSTFLOAT StkFloat
#endif  

typedef long double quad;

#define faustmax(x,y) (((x)>(y)) ? (x) : (y))
#define faustmin(x,y) (((x)<(y)) ? (x) : (y))
