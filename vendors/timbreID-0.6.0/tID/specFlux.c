/*

specFlux - A non-real-time spectral flux analysis external.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

version 0.0.5, December 23, 2011

¥ 0.0.5 incorporates the tIDLib.h header.  corrected a bug in 0.0.4 that didn't reset the windowFuncPtr for the 2nd analysis window
¥ 0.0.4 changed windowing functions so that they are computed ahead of time, this required considerable changes to the windowing stuff since 0.0.3 and before.  changed _bang() to remove needless endSamp calculation and pass lengthSamp rather than window to _analyze() so that windowing will not cover the zero padded section.  made power spectrum computation the first step, and changed the squaring function to a magnitude function instead.  in the case that power spectrum is used, this saves needless computation of sqrt and subsequent squaring. wherever possible, using getbytes() directly instead of getting 0 bytes and resizing.
¥Ê0.0.3 added an ifndef M_PI for guaranteed windows compilation
¥ 0.0.2 adds a #define M_PI for windows compilation, and declares all functions except _setup static

*/

#include "tIDLib.h"

static t_class *specFlux_class;

typedef struct _specFlux
{
    t_object x_obj;
    t_float sr;
	t_float window;
	int powerSpectrum;
	int windowFunction;
	int windowFuncSize;
	int maxWindowSize;
	int *powersOfTwo;
    int powTwoArrSize;
    int separation;
    int squaredDiff;
	int normalize;
	t_float *blackman;
	t_float *cosine;
	t_float *hamming;
	t_float *hann;
	t_word *x_vec;
	t_symbol *x_arrayname;
	int x_arrayPoints;
    t_outlet *x_flux;
    t_outlet *x_fluxList;
} t_specFlux;


/* ------------------------ specFlux -------------------------------- */

static void specFlux_analyze(t_specFlux *x, t_floatarg start, t_floatarg n)
{
	int i, j, window, windowHalf, rearStartSamp, rearEndSamp, startSamp, endSamp, lengthSamp;
	t_float flux, *windowFuncPtr;
    t_atom *listOut;
    t_sample *signal2_R, *signal2_I, *signal1_R, *signal1_I;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for specFlux", x->x_arrayname->s_name);
	else
	{
	
	startSamp = start;
	startSamp = (startSamp<0)?0:startSamp;

	if(n)
		endSamp = startSamp + n-1;
	else
		endSamp = startSamp + x->window-1;
 
	endSamp = (endSamp>=x->x_arrayPoints)?x->x_arrayPoints-1:endSamp;

	lengthSamp = endSamp - startSamp + 1;

	if(endSamp <= startSamp)
	{
		error("bad range of samples.");
		return;
	}
		
	if(lengthSamp > x->powersOfTwo[x->powTwoArrSize-1])
	{
		post("WARNING: specFlux: window truncated because requested size is larger than the current max_window setting. Use the max_window method to allow larger windows. Sizes of more than 131072 may produce unreliable results.");
		lengthSamp = x->powersOfTwo[x->powTwoArrSize-1];
		window = lengthSamp;
		endSamp = startSamp + window - 1;
	}
	else
	{
		i=0;
		while(lengthSamp > x->powersOfTwo[i])
			i++;
		
		window = x->powersOfTwo[i];
	}

 	windowHalf = window * 0.5;
 
	if(x->window != window)
		x->window = window;

	if(x->separation>window)
		x->separation = window*0.25;

	// create local memory
	listOut = (t_atom *)t_getbytes((windowHalf+1)*sizeof(t_atom));
	signal2_R = (t_sample *)t_getbytes(window*sizeof(t_sample));
	signal2_I = (t_sample *)t_getbytes((windowHalf+1)*sizeof(t_sample));
	signal1_R = (t_sample *)t_getbytes(window*sizeof(t_sample));
	signal1_I = (t_sample *)t_getbytes((windowHalf+1)*sizeof(t_sample));

	if(x->windowFuncSize != lengthSamp)
	{
		x->blackman = (t_float *)t_resizebytes(x->blackman, x->windowFuncSize*sizeof(t_float), lengthSamp*sizeof(t_float));
		x->cosine = (t_float *)t_resizebytes(x->cosine, x->windowFuncSize*sizeof(t_float), lengthSamp*sizeof(t_float));
		x->hamming = (t_float *)t_resizebytes(x->hamming, x->windowFuncSize*sizeof(t_float), lengthSamp*sizeof(t_float));
		x->hann = (t_float *)t_resizebytes(x->hann, x->windowFuncSize*sizeof(t_float), lengthSamp*sizeof(t_float));

		x->windowFuncSize = lengthSamp;

		tIDLib_blackmanWindow(x->blackman, x->windowFuncSize);
		tIDLib_cosineWindow(x->cosine, x->windowFuncSize);
		tIDLib_hammingWindow(x->hamming, x->windowFuncSize);
		tIDLib_hannWindow(x->hann, x->windowFuncSize);
	}

	rearStartSamp = startSamp - x->separation;
	rearStartSamp = (rearStartSamp<0)?0:rearStartSamp;

	rearEndSamp = endSamp - x->separation;
	rearEndSamp = (rearEndSamp>=x->x_arrayPoints)?x->x_arrayPoints-1:rearEndSamp;

	// construct the rear analysis window
	for(i=0, j=rearStartSamp; j<=rearEndSamp; i++, j++)
		signal1_R[i] = x->x_vec[j].w_float;

	// set window
	windowFuncPtr = x->hann; //default case to get rid of compile warning

	switch(x->windowFunction)
	{
		case 0:
			break;
		case 1:
			windowFuncPtr = x->blackman;
			break;
		case 2:
			windowFuncPtr = x->cosine;
			break;
		case 3:
			windowFuncPtr = x->hamming;
			break;
		case 4:
			windowFuncPtr = x->hann;
			break;
		default:
			break;
	};

	// if windowFunction == 0, skip the windowing (rectangular)
	if(x->windowFunction>0)
		for(i=0; i<lengthSamp; i++, windowFuncPtr++)
			signal1_R[i] *= *windowFuncPtr;

	// then zero pad the end
	for(; i<window; i++)
		signal1_R[i] = 0.0;

	mayer_realfft(window, signal1_R);
	tIDLib_realfftUnpack(window, windowHalf, signal1_R, signal1_I);
	tIDLib_power(windowHalf+1, signal1_R, signal1_I);

	if(!x->powerSpectrum)
		tIDLib_mag(windowHalf+1, signal1_R);

	if(x->normalize)
		tIDLib_normal(windowHalf+1, signal1_R);

	// construct the forward analysis window
	for(i=0, j=startSamp; j<=endSamp; i++, j++)
		signal2_R[i] = x->x_vec[j].w_float;

	// set window
	windowFuncPtr = x->hann; //default case to get rid of compile warning

	switch(x->windowFunction)
	{
		case 0:
			break;
		case 1:
			windowFuncPtr = x->blackman;
			break;
		case 2:
			windowFuncPtr = x->cosine;
			break;
		case 3:
			windowFuncPtr = x->hamming;
			break;
		case 4:
			windowFuncPtr = x->hann;
			break;
		default:
			break;
	};

	// if windowFunction == 0, skip the windowing (rectangular)
	if(x->windowFunction>0)
		for(i=0; i<lengthSamp; i++, windowFuncPtr++)
			signal2_R[i] *= *windowFuncPtr;

	// then zero pad the end
	for(; i<window; i++)
		signal2_R[i] = 0.0;

	mayer_realfft(window, signal2_R);
	tIDLib_realfftUnpack(window, windowHalf, signal2_R, signal2_I);
	tIDLib_power(windowHalf+1, signal2_R, signal2_I);

	if(!x->powerSpectrum)
		tIDLib_mag(windowHalf+1, signal2_R);

	if(x->normalize)
		tIDLib_normal(windowHalf+1, signal2_R);
	
	flux=0;

	for(i=0; i<=windowHalf; i++)
	{
		t_float diff, val;

		diff = signal2_R[i] - signal1_R[i];
		
		if(x->squaredDiff)
			val = diff*diff;
		else
			val = fabs(diff);
			
		SETFLOAT(listOut+i, diff);
		flux += val;
	}
		
 	outlet_list(x->x_fluxList, 0, windowHalf+1, listOut);
	outlet_float(x->x_flux, flux);

	// free local memory
	t_freebytes(listOut, (windowHalf+1) * sizeof(t_atom));
	t_freebytes(signal1_R, window * sizeof(t_sample));
	t_freebytes(signal1_I, (windowHalf+1) * sizeof(t_sample));
	t_freebytes(signal2_R, window * sizeof(t_sample));
	t_freebytes(signal2_I, (windowHalf+1) * sizeof(t_sample));
	}
}


// analyze the whole damn array
static void specFlux_bang(t_specFlux *x)
{
	int window, startSamp, lengthSamp;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for specFlux", x->x_arrayname->s_name);
	else
	{

	startSamp = 0;
	lengthSamp = x->x_arrayPoints;

	if(lengthSamp > x->powersOfTwo[x->powTwoArrSize-1])
	{
		post("WARNING: specFlux: window truncated because requested size is larger than the current max_window setting. Use the max_window method to allow larger windows. Sizes of more than 131072 may produce unreliable results.");
		lengthSamp = x->powersOfTwo[x->powTwoArrSize-1];
		window = lengthSamp;
	}

	specFlux_analyze(x, startSamp, lengthSamp);

	}
}


static void specFlux_set(t_specFlux *x, t_symbol *s)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(s, garray_class)))
		pd_error(x, "%s: no such array", s->s_name);
	else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
		pd_error(x, "%s: bad template for specFlux", s->s_name);
	else
	    x->x_arrayname = s;
}


static void specFlux_print(t_specFlux *x)
{
	post("samplerate: %f", x->sr);
	post("window: %f", x->window);

	post("squared difference: %i", x->squaredDiff);
	post("power spectrum: %i", x->powerSpectrum);
	post("window function: %i", x->windowFunction);
}


static void specFlux_samplerate(t_specFlux *x, t_floatarg sr)
{
	if(sr<64)
		x->sr = 64;
	else
		x->sr = sr;
}


static void specFlux_max_window(t_specFlux *x, t_floatarg w)
{
	int i;

	if(w<64)
		x->maxWindowSize = 64;
	else
		x->maxWindowSize = w;

	x->powersOfTwo = (int *)t_resizebytes(x->powersOfTwo, x->powTwoArrSize*sizeof(int), sizeof(int));

	x->powersOfTwo[0] = 64; // must have at least this large of a window

	i=1;
	while(x->powersOfTwo[i-1] < x->maxWindowSize)
	{
		x->powersOfTwo = (int *)t_resizebytes(x->powersOfTwo, (i)*sizeof(int), (i+1)*sizeof(int));
		x->powersOfTwo[i] = pow(2, i+6); // +6 because we're starting at 2**6
		i++;
	}

	x->powTwoArrSize = i;

	post("maximum window size: %i", x->maxWindowSize);
}


static void specFlux_windowFunction(t_specFlux *x, t_floatarg f)
{
    f = (f<0)?0:f;
    f = (f>4)?4:f;
	x->windowFunction = f;

	switch(x->windowFunction)
	{
		case 0:
			post("window function: rectangular.");
			break;
		case 1:
			post("window function: blackman.");
			break;
		case 2:
			post("window function: cosine.");
			break;
		case 3:
			post("window function: hamming.");
			break;
		case 4:
			post("window function: hann.");
			break;
		default:
			break;
	};
}


// magnitude spectrum == 0, power spectrum == 1
static void specFlux_powerSpectrum(t_specFlux *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum for specFlux computation.");
	else
		post("using magnitude spectrum for specFlux computation.");
}


static void specFlux_separation(t_specFlux *x, t_floatarg s)
{
	x->separation = s;

	if(x->separation > x->window)
	{
		error("analysis windows cannot be more than %i samples apart", x->window);
		x->separation = x->window*0.25;
	}
	else if(x->separation < 0)
	{
		error("frame separation must be > 0");
		x->separation = x->window*0.25;
	};
		
    post("frame separation: %i", x->separation);

}


static void specFlux_squaredDiff(t_specFlux *x, t_floatarg sd)
{
    sd = (sd<0)?0:sd;
    sd = (sd>1)?1:sd;
	x->squaredDiff = sd;
	
	if(x->squaredDiff)
		post("difference**2");
	else
		post("|difference|");
}


static void specFlux_normalize(t_specFlux *x, t_floatarg norm)
{
    norm = (norm<0)?0:norm;
    norm = (norm>1)?1:norm;
	x->normalize = norm;
	
	if(x->normalize)
		post("spectrum normalization ON.");
	else
		post("spectrum normalization OFF.");
}


static void *specFlux_new(t_symbol *s, t_floatarg separation)
{
    t_specFlux *x = (t_specFlux *)pd_new(specFlux_class);
	int i;
	t_garray *a;

	x->x_flux = outlet_new(&x->x_obj, &s_float);
	x->x_fluxList = outlet_new(&x->x_obj, gensym("list"));

	if(separation)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for specFlux", x->x_arrayname->s_name);

		x->separation = separation;
		
		if(x->separation < 0)
		{
			error("frame separation must be > 0");
			x->separation = 256;
		};

	}
	else if(s)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for specFlux", x->x_arrayname->s_name);

		x->separation = 256;
	}
	else
	{
		error("specFlux: no array specified.");
		x->separation = 256;
	}

	x->sr = 44100.0;
	x->window = 1; // should be a bogus size initially to force the proper resizes when a real _analyze request comes through
	x->windowFuncSize = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0; // choose mag (0) or power (1) spec in the specFlux computation
	x->squaredDiff = 0; // absolute value by default
	x->normalize = 1;
	
	x->maxWindowSize = MAXWINDOWSIZE; // this seems to be the maximum size allowable by mayer_realfft();
	x->powersOfTwo = (int *)t_getbytes(sizeof(int));

	x->powersOfTwo[0] = 64; // must have at least this large of a window

	i=1;
	while(x->powersOfTwo[i-1] < x->maxWindowSize)
	{
		x->powersOfTwo = (int *)t_resizebytes(x->powersOfTwo, i*sizeof(int), (i+1)*sizeof(int));
		x->powersOfTwo[i] = pow(2, i+6); // +6 because we're starting at 2**6
		i++;
	}

	x->powTwoArrSize = i;

  	x->blackman = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));
  	x->cosine = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));
  	x->hamming = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));
  	x->hann = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));

 	// initialize signal windowing functions
	tIDLib_blackmanWindow(x->blackman, x->windowFuncSize);
	tIDLib_cosineWindow(x->cosine, x->windowFuncSize);
	tIDLib_hammingWindow(x->hamming, x->windowFuncSize);
	tIDLib_hannWindow(x->hann, x->windowFuncSize);

    return (x);
}


static void specFlux_free(t_specFlux *x)
{
	// free the window memory
    t_freebytes(x->blackman, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->cosine, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->hamming, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->hann, x->windowFuncSize*sizeof(t_float));

	// free the powers of two table
    t_freebytes(x->powersOfTwo, x->powTwoArrSize*sizeof(int));
}


void specFlux_setup(void)
{
    specFlux_class =
    class_new(
    	gensym("specFlux"),
    	(t_newmethod)specFlux_new,
    	(t_method)specFlux_free,
        sizeof(t_specFlux),
        CLASS_DEFAULT,
        A_DEFSYM,
        A_DEFFLOAT,
		0
    );

	class_addbang(specFlux_class, specFlux_bang);

	class_addmethod(
		specFlux_class,
        (t_method)specFlux_analyze,
		gensym("analyze"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class,
		(t_method)specFlux_set,
		gensym("set"),
		A_SYMBOL,
		0
	);

	class_addmethod(
		specFlux_class,
		(t_method)specFlux_print,
		gensym("print"),
		0
	);

	class_addmethod(
		specFlux_class,
        (t_method)specFlux_samplerate,
		gensym("samplerate"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class,
        (t_method)specFlux_max_window,
		gensym("max_window"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class,
        (t_method)specFlux_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class,
        (t_method)specFlux_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class, 
        (t_method)specFlux_squaredDiff, 
		gensym("squared_diff"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class, 
        (t_method)specFlux_separation,
		gensym("separation"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_class, 
        (t_method)specFlux_normalize,
		gensym("normalize"),
		A_DEFFLOAT,
		0
	);
}

