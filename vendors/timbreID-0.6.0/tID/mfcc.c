/*

mfcc - A non-real-time Mel frequency cepstrum analysis external.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.0.9, December 20, 2011

¥ 0.0.9 incorporates the tIDLib.h header
¥ 0.0.8 fixed a memory leak: wasn't freeing memory for x_filterbank_sizes. not recreating the filterbank each time _analyze is called - only if the window size changes. changed windowing functions so that they are computed ahead of time, this required considerable changes to the windowing stuff since 0.0.6 and before.  changed _bang() to remove needless end_samp calculation and pass length_samp rather than window to _analyze() so that windowing will not cover the zero padded section.  made power spectrum computation the default, and changed the squaring function to a magnitude function instead.  in the case that power spectrum is used, this saves needless computation of sqrt and subsequent squaring. wherever possible, using getbytes() directly instead of getting 0 bytes and resizing.
¥ 0.0.7 fixed a memory issue in _make_filterbank(): was freeing window*sizeof(float) bytes instead of window_half*sizeof(float) bytes
¥ 0.0.6 decided to go with a switch for either summing power in filters, or averaging. default is summing.
¥ 0.0.5 changed filterbank_multiply so that sum of power in each filter is divided by the number of points in the filter.
¥Ê0.0.4 added an ifndef M_PI for guaranteed windows compilation
¥ 0.0.3 adds a #define M_PI for windows compilation, and declares all functions except _setup static
¥ 0.0.2 is part of the update that ensures all function names are prepended by the external name (mfcc_ or mfcc_tilde_, etc).

*/

#include "tIDLib.h"

static t_class *mfcc_class;

typedef struct _mfcc
{
    t_object x_obj;
    t_float sr;
	t_float window;
	int filterAvg;
	int powerSpectrum;
	int normalize;
	int windowFunction;
	int windowFuncSize;
	int sizeFilterFreqs;
	int maxWindowSize;
	int *powersOfTwo;
    int powTwoArrSize;
    int numFilters;
    t_float melSpacing;
	t_float *x_filterFreqs;
	t_filter *x_filterbank;
	t_sample *signal_R;
	t_float *blackman;
	t_float *cosine;
	t_float *hamming;
	t_float *hann;
    t_float *mfcc;
	t_word *x_vec;
	t_symbol *x_arrayname;
	int x_arrayPoints;
    t_outlet *x_featureList;
} t_mfcc;


/* ------------------------ mfcc -------------------------------- */



static void mfcc_create_filterbank(t_mfcc *x, t_floatarg ms)
{
	int i, oldNumFilters;

	x->melSpacing = ms;
	oldNumFilters = x->numFilters;

	x->numFilters = tIDLib_getMelFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->melSpacing, x->sr);

	x->sizeFilterFreqs = x->numFilters+2;

	x->mfcc = (t_float *)t_resizebytes(x->mfcc, oldNumFilters*sizeof(t_float), x->numFilters*sizeof(t_float));

	for(i=0; i<x->numFilters; i++)
		x->mfcc[i] = 0.0;

	tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);
}


static void mfcc_analyze(t_mfcc *x, t_floatarg start, t_floatarg n)
{
	int i, j, oldWindow, window, windowHalf, startSamp, endSamp, lengthSamp;
	t_float *windowFuncPtr;
	t_atom *listOut;
	t_sample *signal_I;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for mfcc", x->x_arrayname->s_name);
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
		post("WARNING: mfcc: window truncated because requested size is larger than the current max_window setting. Use the max_window method to allow larger windows.");
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
	    int oldNumFilters;

		oldWindow = x->window;
		x->window = window;

		oldNumFilters = x->numFilters;
		x->numFilters = tIDLib_getMelFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->melSpacing, x->sr);

		x->sizeFilterFreqs = x->numFilters+2;

	    x->mfcc = (t_float *)t_resizebytes(x->mfcc, oldNumFilters*sizeof(t_float), x->numFilters*sizeof(t_float));

	    for(i=0; i<x->numFilters; i++)
		    x->mfcc[i] = 0.0;

		tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);

		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, oldWindow*sizeof(t_sample), window*sizeof(t_sample));
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
	listOut = (t_atom *)t_getbytes(x->numFilters*sizeof(t_atom));
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

	tIDLib_filterbankMultiply(x->signal_R, x->normalize, x->filterAvg, x->x_filterbank, x->numFilters);
    tIDLib_cosineTransform(x->mfcc, x->signal_R, x->numFilters);

	for(i=0; i<x->numFilters; i++)
		SETFLOAT(listOut+i, x->mfcc[i]);

	outlet_list(x->x_featureList, 0, x->numFilters, listOut);

	// free local memory
	t_freebytes(listOut, x->numFilters*sizeof(t_atom));
	t_freebytes(signal_I, (windowHalf+1)*sizeof(t_sample));
	}
}


// analyze the whole damn array
static void mfcc_bang(t_mfcc *x)
{
	int window, startSamp, lengthSamp;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for mfcc", x->x_arrayname->s_name);
	else
	{

	startSamp = 0;
	lengthSamp = x->x_arrayPoints;

	if(lengthSamp > x->powersOfTwo[x->powTwoArrSize-1])
	{
		post("WARNING: mfcc: window truncated because requested size is larger than the current max_window setting. Use the max_window method to allow larger windows. Sizes of more than 131072 may produce unreliable results.");
		lengthSamp = x->powersOfTwo[x->powTwoArrSize-1];
		window = lengthSamp;
	}

	mfcc_analyze(x, startSamp, lengthSamp);

	}
}


static void mfcc_set(t_mfcc *x, t_symbol *s)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(s, garray_class)))
		pd_error(x, "%s: no such array", s->s_name);
	else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
		pd_error(x, "%s: bad template for mfcc", s->s_name);
	else
	    x->x_arrayname = s;
}


static void mfcc_print(t_mfcc *x)
{
	post("samplerate: %f", x->sr);
	post("window: %f", x->window);

	post("mel spacing: %f", x->melSpacing);
	post("power spectrum: %i", x->powerSpectrum);
	post("normalize: %i", x->normalize);
	post("filter averaging: %i", x->filterAvg);
	post("window function: %i", x->windowFunction);

	post("no. of filters: %i", x->numFilters);
}


static void mfcc_hat(t_mfcc *x, t_floatarg filt)
{
	int i, idx;

	idx = filt;

	if(idx>=x->numFilters)
		error("filter %i does not exist.", idx);
	else if(idx < 0)
		error("filter %i does not exist.", idx);
	else
	{
		post("size[%i]: %i", idx, x->x_filterbank[idx].size);
		for(i=0; i<x->x_filterbank[idx].size; i++)
			post("val %i: %f", i, x->x_filterbank[idx].filter[i]);

		post("idxLo: %i, idxHi: %i", x->x_filterbank[idx].indices[0], x->x_filterbank[idx].indices[1]);
	}
}


static void mfcc_samplerate(t_mfcc *x, t_floatarg sr)
{
	int i, oldNumFilters;

	if(sr<64)
		x->sr = 64;
	else
		x->sr = sr;

	oldNumFilters = x->numFilters;

	x->numFilters = tIDLib_getMelFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->melSpacing, x->sr);

	x->sizeFilterFreqs = x->numFilters+2;

	x->mfcc = (t_float *)t_resizebytes(x->mfcc, oldNumFilters*sizeof(t_float), x->numFilters*sizeof(t_float));

	for(i=0; i<x->numFilters; i++)
		x->mfcc[i] = 0.0;

	tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);
}


static void mfcc_max_window(t_mfcc *x, t_floatarg w)
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


static void mfcc_windowFunction(t_mfcc *x, t_floatarg f)
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


static void mfcc_filter_avg(t_mfcc *x, t_floatarg avg)
{
    avg = (avg<0)?0:avg;
    avg = (avg>1)?1:avg;
	x->filterAvg = avg;

	if(x->filterAvg)
		post("averaging energy in filters.");
	else
		post("summing energy in filters.");
}


// magnitude spectrum == 0, power spectrum == 1
static void mfcc_powerSpectrum(t_mfcc *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum for mfcc computation.");
	else
		post("using magnitude spectrum for mfcc computation.");
}


static void mfcc_normalize(t_mfcc *x, t_floatarg norm)
{
    norm = (norm<0)?0:norm;
    norm = (norm>1)?1:norm;
	x->normalize = norm;

	if(x->normalize)
		post("spectrum normalization ON.");
	else
		post("spectrum normalization OFF.");
}


static void *mfcc_new(t_symbol *s, t_floatarg melSpacing)
{
    t_mfcc *x = (t_mfcc *)pd_new(mfcc_class);
	int i;
	t_garray *a;

	x->x_featureList = outlet_new(&x->x_obj, gensym("list"));

	if(melSpacing)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for mfcc", x->x_arrayname->s_name);

		x->melSpacing = melSpacing;
	}
	else if(s)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_arrayPoints, &x->x_vec))
	    	pd_error(x, "%s: bad template for mfcc", x->x_arrayname->s_name);

		x->melSpacing = 100;
	}
	else
	{
		error("mfcc: no array specified.");
		x->melSpacing = 100;
	}

	x->sr = 44100.0;
	x->window = 1; // should be a bogus size initially to force the proper resizes when a real _analyze request comes through
	x->windowFuncSize = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0; // choose mag (0) or power (1) spec in the mfcc computation
	x->normalize = 1; // this is generally a good thing, but should be off for concatenative synth
	x->filterAvg = 0;
	x->numFilters = 0; // this is just an init size that will be updated in createFilterbank anyway. comes through

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

	// grab memory
	x->x_filterbank = (t_filter *)t_getbytes(0);
	x->x_filterFreqs = (t_float *)t_getbytes(0);

	x->numFilters = tIDLib_getMelFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->melSpacing, x->sr);

	x->sizeFilterFreqs = x->numFilters+2;

	x->mfcc = (t_float *)t_getbytes(x->numFilters*sizeof(t_float));

	for(i=0; i<x->numFilters; i++)
		x->mfcc[i] = 0.0;

	tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, 0, x->numFilters, x->window, x->sr);

    return (x);
}


static void mfcc_free(t_mfcc *x)
{
	int i;

	// free the input buffer memory
    t_freebytes(x->signal_R, x->window*sizeof(t_sample));

	// free the mfcc memory
    t_freebytes(x->mfcc, x->numFilters*sizeof(t_float));

	// free the window memory
    t_freebytes(x->blackman, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->cosine, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->hamming, x->windowFuncSize*sizeof(t_float));
    t_freebytes(x->hann, x->windowFuncSize*sizeof(t_float));

    // free filterFreqs memory
    t_freebytes(x->x_filterFreqs, x->sizeFilterFreqs*sizeof(t_float));

	// free the filterbank memory
	for(i=0; i<x->numFilters; i++)
		t_freebytes(x->x_filterbank[i].filter, x->x_filterbank[i].size*sizeof(t_float));

	t_freebytes(x->x_filterbank, x->numFilters*sizeof(t_filter));

	// free the powers of two table
    t_freebytes(x->powersOfTwo, x->powTwoArrSize*sizeof(int));
}


void mfcc_setup(void)
{
    mfcc_class =
    class_new(
    	gensym("mfcc"),
    	(t_newmethod)mfcc_new,
    	(t_method)mfcc_free,
        sizeof(t_mfcc),
        CLASS_DEFAULT,
        A_DEFSYM,
		A_DEFFLOAT,
		0
    );

	class_addbang(mfcc_class, mfcc_bang);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_analyze,
		gensym("analyze"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
		(t_method)mfcc_set,
		gensym("set"),
		A_SYMBOL,
		0
	);

	class_addmethod(
		mfcc_class,
		(t_method)mfcc_print,
		gensym("print"),
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_create_filterbank,
		gensym("filterbank"),
        A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_hat,
		gensym("hat"),
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_samplerate,
		gensym("samplerate"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_max_window,
		gensym("max_window"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_filter_avg,
		gensym("filter_avg"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		mfcc_class,
        (t_method)mfcc_normalize,
		gensym("normalize"),
		A_DEFFLOAT,
		0
	);
}

