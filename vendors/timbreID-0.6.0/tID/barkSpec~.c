/*

barkSpec~ - A Bark Frequency Spectrum Analysis external.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.2.9D, November 28, 2011

¥ 0.2.9D finally got memory resizing in tIDLib header to work. Also including Nyquist now just for good measure.  Bringing back overlap, since it's the only way to inform the object of something like [block~ 1024 4], where the sampling rate will appear as 4x its actual rate.  And sampling rate affects filterbank construction....
¥ 0.2.9C lost track of changes, but bottom line is that x->x_filterbankSizes memory resizing isn't working in create_filterbank.
¥ 0.2.9B changing variable names to cut down on underscores
¥ 0.2.9 employing the timbreID.h header in preparation for timbreID-0.6.0.  barkSpec~.c is the first attempt to integrate this.  Going to get rid of the x->overlap variable too.  Not necessary anymore.
¥ 0.2.8 fixed a memory leak: wasn't freeing memory for x_filterbankSizes. made power spectrum computation the default, and changed the squaring function to a magnitude function instead.  in the case that power spectrum is used, this saves needless computation of sqrt and subsequent squaring. wherever possible, using t_getbytes() directly instead of getting 0 bytes and resizing. corrected createFilterbank declaration in _setup to show that it only has one argument, not two.
¥ 0.2.7 fixed a memory issue in _make_filterbank(): was freeing window*sizeof(float) bytes instead of window_half*sizeof(float) bytes
¥ 0.2.6 decided to go with a switch for either summing power in filters, or averaging. default is summing.
¥ 0.2.5 changed filterbank_multiply so that sum of power in each filter is divided by the number of points in the filter.
¥Ê0.2.4 added an ifndef M_PI for guaranteed windows compilation
¥ 0.2.3 adds a #define M_PI for windows compilation, and declares all functions except _setup static
¥ 0.2.2 is part of the update that ensures all function names are prepended by the external name (bfcc_ or bfcc_tilde_, etc).
¥ 0.2.1 uses dynamic memory allocation for filter widths
¥ 0.2.0 implements mayer_realfft
¥ 0.1.9 added normalization option

*/

#include "tIDLib.h"

static t_class *barkSpec_tilde_class;

typedef struct _barkSpec_tilde
{
	t_object x_obj;
	t_float sr;
	t_float n;
	t_float window;
	int overlap;
	int powerSpectrum;
	int normalize;
	int filterAvg;
	int windowFunction;
	double lastDspTime;
	int sizeFilterFreqs;
	int numFilters;
	t_float barkSpacing;
	t_float *x_filterFreqs;
	t_filter *x_filterbank;
	t_sample *signal_R;
	t_float *blackman;
	t_float *cosine;
	t_float *hamming;
	t_float *hann;
	t_outlet *x_featureList;
	t_float x_f;

} t_barkSpec_tilde;


/* ------------------------ barkSpec~ -------------------------------- */

static void barkSpec_tilde_bang(t_barkSpec_tilde *x)
{
	int i, j, window, windowHalf, bangSample;
	t_atom *listOut;
	t_sample *signal_R, *signal_I;
	t_float *windowFuncPtr;
	double currentTime;

	window = x->window;
	windowHalf = window*0.5;

	// create local memory
	listOut = (t_atom *)t_getbytes(x->numFilters*sizeof(t_atom));
	signal_R = (t_sample *)t_getbytes(window*sizeof(t_sample));
	signal_I = (t_sample *)t_getbytes((windowHalf+1)*sizeof(t_sample));

	currentTime = clock_gettimesince(x->lastDspTime);
	bangSample = (int)(((currentTime/1000.0)*x->sr)+0.5); // round

	if (bangSample < 0)
		bangSample = 0;
	else if ( bangSample >= x->n )
		bangSample = x->n - 1;

	// construct analysis window using bangSample as the end of the window
	for(i=0, j=bangSample; i<window; i++, j++)
		signal_R[i] = x->signal_R[j];

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
		for(i=0; i<window; i++, windowFuncPtr++)
			signal_R[i] *= *windowFuncPtr;

	mayer_realfft(window, signal_R);
	tIDLib_realfftUnpack(window, windowHalf, signal_R, signal_I);
	tIDLib_power(windowHalf+1, signal_R, signal_I);

	// power spectrum sometimes generates lower scores than magnitude. make it optional.
	if(!x->powerSpectrum)
		tIDLib_mag(windowHalf+1, signal_R);

	tIDLib_filterbankMultiply(signal_R, x->normalize, x->filterAvg, x->x_filterbank, x->numFilters);

	for(i=0; i<x->numFilters; i++)
		SETFLOAT(listOut+i, signal_R[i]);

	outlet_list(x->x_featureList, 0, x->numFilters, listOut);

	// free local memory
	t_freebytes(listOut, x->numFilters*sizeof(t_atom));
	t_freebytes(signal_R, window*sizeof(t_sample));
	t_freebytes(signal_I, (windowHalf+1)*sizeof(t_sample));
}


static void barkSpec_tilde_createFilterbank(t_barkSpec_tilde *x, t_floatarg bs)
{
	int oldNumFilters;

	x->barkSpacing = bs;
	oldNumFilters = x->numFilters;

	x->numFilters = tIDLib_getBarkFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->barkSpacing, x->sr);

	x->sizeFilterFreqs = x->numFilters+2;

	tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);
}


static void barkSpec_tilde_filterFreqs(t_barkSpec_tilde *x)
{
	int i;

	for(i=0; i<x->numFilters+2; i++)
		post("filterFreqs[%i]: %f", i, x->x_filterFreqs[i]);
}


static void barkSpec_tilde_print(t_barkSpec_tilde *x)
{
	post("samplerate: %f", x->sr);
	post("window: %f", x->window);

	post("bark spacing: %f", x->barkSpacing);
	post("power spectrum: %i", x->powerSpectrum);
	post("normalize: %i", x->normalize);
	post("filter averaging: %i", x->filterAvg);
	post("window function: %i", x->windowFunction);

	post("no. of filters: %i", x->numFilters);
}


static void barkSpec_tilde_hat(t_barkSpec_tilde *x, t_floatarg filt)
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


static void barkSpec_tilde_window(t_barkSpec_tilde *x, t_floatarg ws)
{
	int i, isPow2, window, oldNumFilters;

	window = ws;

	isPow2 = window && !( (window-1) & window );

	if( !isPow2 )
		error("requested window size is not a power of 2.");
	else
	{
		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, (x->window+x->n)*sizeof(t_sample), (window+x->n)*sizeof(t_sample));
		x->blackman = (t_float *)t_resizebytes(x->blackman, x->window*sizeof(t_float), window*sizeof(t_float));
		x->cosine = (t_float *)t_resizebytes(x->cosine, x->window*sizeof(t_float), window*sizeof(t_float));
		x->hamming = (t_float *)t_resizebytes(x->hamming, x->window*sizeof(t_float), window*sizeof(t_float));
		x->hann = (t_float *)t_resizebytes(x->hann, x->window*sizeof(t_float), window*sizeof(t_float));

		x->window = (t_float)window;

		// re-init window functions
		tIDLib_blackmanWindow(x->blackman, x->window);
		tIDLib_cosineWindow(x->cosine, x->window);
		tIDLib_hammingWindow(x->hamming, x->window);
		tIDLib_hannWindow(x->hann, x->window);

		// init signal buffer
		for(i=0; i<x->window+x->n; i++)
			x->signal_R[i] = 0.0;

		oldNumFilters = x->numFilters;
		x->numFilters = tIDLib_getBarkFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->barkSpacing, x->sr);

		x->sizeFilterFreqs = x->numFilters+2;

		tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);

		post("window size: %i", (int)x->window);
	}
}


static void barkSpec_tilde_overlap(t_barkSpec_tilde *x, t_floatarg o)
{
	int overlap;

	overlap = o;

	// this change will be picked up in _dsp, where the filterbank will be recreated based on the samplerate sp[0]->s_sr/x->overlap;
	if(overlap > 0)
		x->overlap = overlap;
	else
		error("overlap must be at least 1.");

    post("overlap: %i", x->overlap);
}


static void barkSpec_tilde_windowFunction(t_barkSpec_tilde *x, t_floatarg f)
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


static void barkSpec_tilde_filterAvg(t_barkSpec_tilde *x, t_floatarg avg)
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
static void barkSpec_tilde_powerSpectrum(t_barkSpec_tilde *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum.");
	else
		post("using magnitude spectrum.");
}


static void barkSpec_tilde_normalize(t_barkSpec_tilde *x, t_floatarg norm)
{
    norm = (norm<0)?0:norm;
    norm = (norm>1)?1:norm;
	x->normalize = norm;

	if(x->normalize)
		post("spectrum normalization ON.");
	else
		post("spectrum normalization OFF.");
}


static void *barkSpec_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
	t_barkSpec_tilde *x = (t_barkSpec_tilde *)pd_new(barkSpec_tilde_class);
	int i, isPow2;
	s=s;

	x->x_featureList = outlet_new(&x->x_obj, gensym("list"));

	if(argc > 1)
	{
		x->window = atom_getfloat(argv);  // should perform a check for >64 && power of two
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );

		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead.");
			x->window = 1024;
		};

		x->barkSpacing = atom_getfloat(argv+1);
	}
	else if(argc > 0)
	{
		x->window = atom_getfloat(argv);
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );

		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead.");
			x->window = 1024;
		};

		x->barkSpacing = 0.5;
	}
	else
	{
		x->window = 1024;
		x->barkSpacing = 0.5;
	}

	x->sr = 44100.0;
	x->n = 64.0;
	x->overlap = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0; // choose mag (0) or power (1) spec
	x->normalize = 1; // this is generally a good thing, but should be off for concatenative synth
	x->filterAvg = 0;
	x->lastDspTime = clock_getlogicaltime();
	x->sizeFilterFreqs = 0;
	x->numFilters = 0; // this is just an init size that will be updated in createFilterbank anyway.

	x->signal_R = (t_sample *)t_getbytes((x->window+x->n)*sizeof(t_sample));

	// initialize signal buffer
	for(i=0; i<x->window+x->n; i++)
		x->signal_R[i] = 0.0;

  	x->blackman = (t_float *)t_getbytes(x->window*sizeof(t_float));
  	x->cosine = (t_float *)t_getbytes(x->window*sizeof(t_float));
  	x->hamming = (t_float *)t_getbytes(x->window*sizeof(t_float));
  	x->hann = (t_float *)t_getbytes(x->window*sizeof(t_float));

 	// initialize signal windowing functions
	tIDLib_blackmanWindow(x->blackman, x->window);
	tIDLib_cosineWindow(x->cosine, x->window);
	tIDLib_hammingWindow(x->hamming, x->window);
	tIDLib_hannWindow(x->hann, x->window);

	// grab memory
	x->x_filterbank = (t_filter *)t_getbytes(0);
	x->x_filterFreqs = (t_float *)t_getbytes(0);

	x->numFilters = tIDLib_getBarkFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->barkSpacing, x->sr);

	x->sizeFilterFreqs = x->numFilters+2;

	tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, 0, x->numFilters, x->window, x->sr);

	return (x);
}


static t_int *barkSpec_tilde_perform(t_int *w)
{
	int i, n;

	t_barkSpec_tilde *x = (t_barkSpec_tilde *)(w[1]);

	t_sample *in = (t_sample *)(w[2]);
	n = w[3];

 	// shift signal buffer contents back.
	for(i=0; i<(x->window-n); i++)
		x->signal_R[i] = x->signal_R[i+n];

	// write new block to end of signal buffer.
	for(i=0; i<n; i++)
		x->signal_R[(int)x->window-n+i] = in[i];

	x->lastDspTime = clock_getlogicaltime();

	return (w+4);
}


static void barkSpec_tilde_dsp(t_barkSpec_tilde *x, t_signal **sp)
{
	int i, oldNumFilters;

	dsp_add(
		barkSpec_tilde_perform,
		3,
		x,
		sp[0]->s_vec,
		sp[0]->s_n
	);

	// compare n to stored n and recalculate filterbank if different
	if( sp[0]->s_sr != (x->sr*x->overlap) || sp[0]->s_n != x->n )
	{
		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, (x->window+x->n) * sizeof(t_sample), (x->window+sp[0]->s_n) * sizeof(t_sample));

		x->sr = sp[0]->s_sr/x->overlap;
		x->n = sp[0]->s_n;

		// init signal buffer
		for(i=0; i<x->window+x->n; i++)
			x->signal_R[i] = 0.0;

		oldNumFilters = x->numFilters;
		x->numFilters = tIDLib_getBarkFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->barkSpacing, x->sr);

		x->sizeFilterFreqs = x->numFilters+2;

		tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);

		post("barkSpec~: window size: %i. sampling rate: %i, block size: %i", (int)x->window, (int)x->sr, (int)x->n);
	};
};


static void barkSpec_tilde_free(t_barkSpec_tilde *x)
{
	int i;

	// free the input buffer memory
	t_freebytes(x->signal_R, (x->window+x->n)*sizeof(t_sample));

	// free the window memory
	t_freebytes(x->blackman, x->window*sizeof(t_float));
	t_freebytes(x->cosine, x->window*sizeof(t_float));
	t_freebytes(x->hamming, x->window*sizeof(t_float));
	t_freebytes(x->hann, x->window*sizeof(t_float));

    // free filterFreqs memory
	t_freebytes(x->x_filterFreqs, x->sizeFilterFreqs*sizeof(t_float));

	// free the filterbank memory
	for(i=0; i<x->numFilters; i++)
		t_freebytes(x->x_filterbank[i].filter, x->x_filterbank[i].size*sizeof(t_float));

	t_freebytes(x->x_filterbank, x->numFilters*sizeof(t_filter));
}


void barkSpec_tilde_setup(void)
{
	barkSpec_tilde_class =
	class_new(
		gensym("barkSpec~"),
		(t_newmethod)barkSpec_tilde_new,
		(t_method)barkSpec_tilde_free,
		sizeof(t_barkSpec_tilde),
		CLASS_DEFAULT,
		A_GIMME,
		0
	);

	CLASS_MAINSIGNALIN(barkSpec_tilde_class, t_barkSpec_tilde, x_f);

	class_addbang(barkSpec_tilde_class, barkSpec_tilde_bang);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_filterFreqs,
		gensym("filter_freqs"),
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_print,
		gensym("print"),
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_createFilterbank,
		gensym("filterbank"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_hat,
		gensym("hat"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_window,
		gensym("window"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
        (t_method)barkSpec_tilde_overlap,
		gensym("overlap"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_filterAvg,
		gensym("filter_avg"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_normalize,
		gensym("normalize"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		barkSpec_tilde_class,
		(t_method)barkSpec_tilde_dsp,
		gensym("dsp"),
		0
	);
}

