/*

zeroCrossing - A non-real-time zero crossing rate analysis external.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.0.5, December 23, 2011

¥ 0.0.5 incorporates the tIDLib.h header
¥ 0.0.4 zeroCrossing shouldn't have had windowing choices in it, but it did in 0.0.3. those are removed here. changed _bang() to remove needless end_samp calculation and pass length_samp rather than window to _analyze() so that windowing will not cover the zero padded section.  wherever possible, using getbytes() directly instead of getting 0 bytes and resizing.
¥ 0.0.3 declares all functions except _setup static
¥ 0.0.2 is part of the update that ensures all function names are prepended by the external name (bfcc_ or bfcc_tilde_, etc).

*/

#include "tIDLib.h"

static t_class *zeroCrossing_class;

typedef struct _zeroCrossing
{
    t_object x_obj;
    t_float sr;
	t_float window;
	t_sample *signal_R;
	t_word *x_vec;
	t_symbol *x_arrayname;
	int x_arrayPoints;
    t_outlet *x_crossings;
} t_zeroCrossing;


/* ------------------------ zeroCrossing -------------------------------- */

static void zeroCrossing_analyze(t_zeroCrossing *x, t_floatarg start, t_floatarg n)
{
	int i, j, oldWindow, window, startSamp, endSamp, crossings;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for zeroCrossing", x->x_arrayname->s_name);
	else
	{

	startSamp = start;
	startSamp = (startSamp<0)?0:startSamp;

	if(n)
		endSamp = startSamp + n-1;
	else
		endSamp = startSamp + x->window-1;

	if(endSamp > x->x_arrayPoints)
		endSamp = x->x_arrayPoints-1;

	window = endSamp - startSamp + 1;

	if(endSamp <= startSamp)
	{
		error("bad range of samples.");
		return;
	}

	if(x->window != window)
	{
		oldWindow = x->window;
		x->window = window;

		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, oldWindow*sizeof(t_sample), x->window*sizeof(t_sample));
	}

	// construct analysis window
	for(i=0, j=startSamp; j<=endSamp; i++, j++)
		x->signal_R[i] = x->x_vec[j].w_float;

	// then zero pad the end
	for(; i<window; i++)
		x->signal_R[i] = 0.0;

	crossings=0;
	crossings = tIDLib_zeroCrossingRate(window, x->signal_R);

	outlet_float(x->x_crossings, crossings);
	}
}


// analyze the whole damn array
static void zeroCrossing_bang(t_zeroCrossing *x)
{
	int window, startSamp, endSamp;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for zeroCrossing", x->x_arrayname->s_name);
	else
	{

	startSamp = 0;
	endSamp = x->x_arrayPoints-1;

	window = x->x_arrayPoints;

	zeroCrossing_analyze(x, startSamp, window);
	}
}


static void zeroCrossing_set(t_zeroCrossing *x, t_symbol *s)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(s, garray_class)))
		pd_error(x, "%s: no such array", s->s_name);
	else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
		pd_error(x, "%s: bad template for zeroCrossing", s->s_name);
	else
	    x->x_arrayname = s;
}


static void *zeroCrossing_new(t_symbol *s)
{
    t_zeroCrossing *x = (t_zeroCrossing *)pd_new(zeroCrossing_class);
	int i;
	t_garray *a;

	x->x_crossings = outlet_new(&x->x_obj, &s_float);
	
	if(s)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for zeroCrossing", x->x_arrayname->s_name);
	}
	else
		error("zeroCrossing: no array specified.");

	x->sr = 44100.0;
	x->window = 1; // should be a bogus size initially to force the proper resizes when a real _analyze request comes through

	x->signal_R = (t_sample *)t_getbytes(x->window*sizeof(t_sample));

	for(i=0; i<x->window; i++)
		x->signal_R[i] = 0.0;

    return (x);
}


static void zeroCrossing_free(t_zeroCrossing *x)
{
	// free the input buffer memory
    t_freebytes(x->signal_R, x->window*sizeof(t_sample));
}


void zeroCrossing_setup(void)
{
    zeroCrossing_class =
    class_new(
    	gensym("zeroCrossing"),
    	(t_newmethod)zeroCrossing_new,
    	(t_method)zeroCrossing_free,
        sizeof(t_zeroCrossing),
        CLASS_DEFAULT,
        A_DEFSYM,
		0
    );

	class_addbang(zeroCrossing_class, zeroCrossing_bang);

	class_addmethod(
		zeroCrossing_class,
        (t_method)zeroCrossing_analyze,
		gensym("analyze"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		zeroCrossing_class,
		(t_method)zeroCrossing_set,
		gensym("set"),
		A_SYMBOL,
		0
	);
}

