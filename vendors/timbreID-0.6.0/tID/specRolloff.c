/*

specRolloff - A non-real-time spectral rolloff analysis external.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

version 0.0.7, December 23, 2011

¥ 0.0.7 incorporates the tIDLib.h header
¥ 0.0.6 changed windowing functions so that they are computed ahead of time, this required considerable changes to the windowing stuff since 0.0.5 and before.  changed _bang() to remove needless end_samp calculation and pass length_samp rather than window to _analyze() so that windowing will not cover the zero padded section.  made power spectrum computation the first step, and changed the squaring function to a magnitude function instead.  in the case that power spectrum is used, this saves needless computation of sqrt and subsequent squaring. wherever possible, using getbytes() directly instead of getting 0 bytes and resizing.
¥Ê0.0.5 added an ifndef M_PI for guaranteed windows compilation
¥ 0.0.4 adds a #define M_PI for windows compilation, and declares all functions except _setup static
¥ 0.0.3 fixes a bug. If a creation argument for concentration isn't given, there was no default value implemented, causing output to be 0.
¥ 0.0.2 adds a break out of the concentration while loop in _analyze. Maybe because of underflow, sometimes energy never got up to its target.

*/

#include "tIDLib.h"

static t_class *specRolloff_class;

typedef struct _specRolloff
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
    t_float *binFreqs;
    t_float concentration;
	t_sample *signal_R;
	t_float *blackman;
	t_float *cosine;
	t_float *hamming;
	t_float *hann;
	t_word *x_vec;
	t_symbol *x_arrayname;
	int x_arrayPoints;
    t_outlet *x_rolloff;
} t_specRolloff;


/* ------------------------ specRolloff -------------------------------- */

static void specRolloff_analyze(t_specRolloff *x, t_floatarg start, t_floatarg n)
{
	int i, j, oldWindow, window, windowHalf, startSamp, endSamp, lengthSamp;
    t_float energy, energyTarget, rolloff, *windowFuncPtr;
	t_sample *signal_I;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for specRolloff", x->x_arrayname->s_name);
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

	lengthSamp = endSamp-startSamp+1;

	if(endSamp <= startSamp)
	{
		error("bad range of samples.");
		return;
	}

	if(lengthSamp > x->powersOfTwo[x->powTwoArrSize-1])
	{
		post("WARNING: specRolloff: window truncated because requested size is larger than the current max_window setting. Use the max_window method to allow larger windows.");
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
	{
		oldWindow = x->window;
		x->window = window;

		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, oldWindow*sizeof(t_sample), x->window*sizeof(t_sample));

		x->binFreqs = (t_float *)t_resizebytes(x->binFreqs, oldWindow*sizeof(t_float), x->window*sizeof(t_float));
	
		// freqs for each bin based on current window size and sample rate
		for(i=0; i<x->window; i++)
			x->binFreqs[i] = (x->sr/x->window)*i;
	}

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

	// create local memory
	signal_I = (t_sample *)t_getbytes((windowHalf+1)*sizeof(t_sample));

	// construct analysis window
	for(i=0, j=startSamp; j<=endSamp; i++, j++)
		x->signal_R[i] = x->x_vec[j].w_float;

	// set window function
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
			x->signal_R[i] *= *windowFuncPtr;

	// then zero pad the end
	for(; i<window; i++)
		x->signal_R[i] = 0.0;

	mayer_realfft(window, x->signal_R);
	tIDLib_realfftUnpack(window, windowHalf, x->signal_R, signal_I);
	tIDLib_power(windowHalf+1, x->signal_R, signal_I);

	// power spectrum sometimes generates lower scores than magnitude. make it optional.
	if(!x->powerSpectrum)
		tIDLib_mag(windowHalf+1, x->signal_R);

	energyTarget=0;
	
	for(i=0; i<=windowHalf; i++)
		energyTarget += x->signal_R[i];
	
	energyTarget *= x->concentration;
		
	energy=0;
	i=0;
	
	while(energy <= energyTarget)
	{
		energy += x->signal_R[i];
		i++;

		if(i>windowHalf)
			break;
	}
	
	if(i==0) i=1;
	
	rolloff = x->binFreqs[i-1]; // back up one because the last one went over...
		
	outlet_float(x->x_rolloff, rolloff);

	// free local memory
	t_freebytes(signal_I, (windowHalf+1)*sizeof(t_sample));
	}
}


// analyze the whole damn array
static void specRolloff_bang(t_specRolloff *x)
{
	int window, startSamp, lengthSamp;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for specRolloff", x->x_arrayname->s_name);
	else
	{

	startSamp = 0;
	lengthSamp = x->x_arrayPoints;

	if(lengthSamp > x->powersOfTwo[x->powTwoArrSize-1])
	{
		post("WARNING: specRolloff: window truncated because requested size is larger than the current max_window setting. Use the max_window method to allow larger windows. Sizes of more than 131072 may produce unreliable results.");
		lengthSamp = x->powersOfTwo[x->powTwoArrSize-1];
		window = lengthSamp;
	}

	specRolloff_analyze(x, startSamp, lengthSamp);

	}
}


static void specRolloff_concentration(t_specRolloff *x, t_floatarg c)
{
	if(c<0 || c>1.0)
		error("concentration must be between 0.0 and 1.0.");
	else
		x->concentration = c;
		
    post("concentration: %f", x->concentration);

}


static void specRolloff_set(t_specRolloff *x, t_symbol *s)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(s, garray_class)))
		pd_error(x, "%s: no such array", s->s_name);
	else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
		pd_error(x, "%s: bad template for specRolloff", s->s_name);
	else
	    x->x_arrayname = s;
}


static void specRolloff_print(t_specRolloff *x)
{
	post("concentration: %f", x->concentration);

	post("samplerate: %f", x->sr);
	post("window: %f", x->window);

	post("power spectrum: %i", x->powerSpectrum);
	post("window function: %i", x->windowFunction);
}


static void specRolloff_samplerate(t_specRolloff *x, t_floatarg sr)
{
	int i;

	if(sr<64)
		x->sr = 64;
	else
		x->sr = sr;

	x->binFreqs = (t_float *)t_getbytes(x->window*sizeof(t_float));

	// freqs for each bin based on current window size and sample rate
	for(i=0; i<x->window; i++)
		x->binFreqs[i] = (x->sr/x->window)*i;
}


static void specRolloff_max_window(t_specRolloff *x, t_floatarg w)
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


static void specRolloff_windowFunction(t_specRolloff *x, t_floatarg f)
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
static void specRolloff_powerSpectrum(t_specRolloff *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum for specRolloff computation.");
	else
		post("using magnitude spectrum for specRolloff computation.");
}


static void *specRolloff_new(t_symbol *s, t_floatarg concentration)
{
    t_specRolloff *x = (t_specRolloff *)pd_new(specRolloff_class);
	int i;
	t_garray *a;

	x->x_rolloff = outlet_new(&x->x_obj, &s_float);
	
	if(concentration)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for specRolloff", x->x_arrayname->s_name);
	    	
		x->concentration = concentration;

		x->concentration = concentration;

		if(x->concentration<0)
		{
			error("concentration must be between 0.0 and 1.0.");
			x->concentration = 0.85;
		}
		else if(x->concentration>1.0)
		{
			error("concentration must be between 0.0 and 1.0.");
			x->concentration = 0.85;
		}
	}
	else if(s)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for specRolloff", x->x_arrayname->s_name);
	    	
	    x->concentration = 0.85;
	}
	else
		error("specRolloff: no array specified.");

	x->sr = 44100.0;
	x->window = 1; // should be a bogus size initially to force the proper resizes when a real _analyze request comes through
	x->windowFuncSize = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0; // choose mag (0) or power (1) spec in the specRolloff computation

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

	x->signal_R = (t_sample *)t_getbytes(x->window*sizeof(t_sample));

	for(i=0; i<x->window; i++)
		x->signal_R[i] = 0.0;

  	x->blackman = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));
  	x->cosine = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));
  	x->hamming = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));
  	x->hann = (t_float *)t_getbytes(x->windowFuncSize*sizeof(t_float));

 	// initialize signal windowing functions
	tIDLib_blackmanWindow(x->blackman, x->windowFuncSize);
	tIDLib_cosineWindow(x->cosine, x->windowFuncSize);
	tIDLib_hammingWindow(x->hamming, x->windowFuncSize);
	tIDLib_hannWindow(x->hann, x->windowFuncSize);

	x->binFreqs = (t_float *)t_getbytes(x->window*sizeof(t_float));

    return (x);
}


static void specRolloff_free(t_specRolloff *x)
{
	// free the input buffer memory
    t_freebytes(x->signal_R, x->window*sizeof(t_sample));

	// free the window memory
    t_freebytes(x->blackman, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->cosine, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->hamming, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->hann, x->windowFuncSize*sizeof(t_float));

	// free the binFreqs memory
    t_freebytes(x->binFreqs, x->window*sizeof(t_sample));

	// free the powers of two table
    t_freebytes(x->powersOfTwo, x->powTwoArrSize*sizeof(int));
}


void specRolloff_setup(void)
{
    specRolloff_class =
    class_new(
    	gensym("specRolloff"),
    	(t_newmethod)specRolloff_new,
    	(t_method)specRolloff_free,
        sizeof(t_specRolloff),
        CLASS_DEFAULT,
        A_DEFSYM,
		A_DEFFLOAT,
		0
    );

	class_addbang(specRolloff_class, specRolloff_bang);

	class_addmethod(
		specRolloff_class, 
        (t_method)specRolloff_concentration,
		gensym("concentration"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_class,
        (t_method)specRolloff_analyze,
		gensym("analyze"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_class,
		(t_method)specRolloff_set,
		gensym("set"),
		A_SYMBOL,
		0
	);

	class_addmethod(
		specRolloff_class,
		(t_method)specRolloff_print,
		gensym("print"),
		0
	);

	class_addmethod(
		specRolloff_class,
        (t_method)specRolloff_samplerate,
		gensym("samplerate"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_class,
        (t_method)specRolloff_max_window,
		gensym("max_window"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_class,
        (t_method)specRolloff_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_class,
        (t_method)specRolloff_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);
}

