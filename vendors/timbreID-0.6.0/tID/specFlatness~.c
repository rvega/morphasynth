/*

specFlatness~

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.2.4, December 21, 2011

� 0.2.4 incorporating tIDLib.h for timbreID-0.6.0 release
� 0.2.3 as part of timbreID-0.5 update, getting rid of unnecessary getbytes(0) calls. also adding power spectrum option. completely removing normalization functions (useless for this relative feature).
��0.2.2 added an ifndef M_PI for guaranteed windows compilation
� 0.2.1 adds a #define M_PI for windows compilation, and declares all functions except _setup static
� 0.2.0 implements mayer_realfft
��Commenting out normalization since it's basically irrelevant in ratio measures lke this.
� 0.1.8 added normalization option

*/

#include "tIDLib.h"

static t_class *specFlatness_tilde_class;

typedef struct _specFlatness_tilde
{
    t_object x_obj;
    t_float sr;
    t_float n;
	int windowFunction;
    int overlap;
    int powerSpectrum;
    int window;
    double lastDspTime;
    t_sample *signal_R;
    t_float *nthRoots;
    t_float *blackman;
    t_float *cosine;
    t_float *hamming;
    t_float *hann;
    t_outlet *x_flatness;
    t_float x_f;

} t_specFlatness_tilde;


/* ------------------------ specFlatness~ -------------------------------- */

static void specFlatness_tilde_bang(t_specFlatness_tilde *x)
{
    int i, j, window, windowHalf, bangSample;
    t_sample *signal_R, *signal_I;
    t_float dividend, divisor, windowHalfPlusOneRecip, flatness, *windowFuncPtr;
	double currentTime;

    window = x->window;
    windowHalf = window*0.5;
    windowHalfPlusOneRecip = 1.0/(t_float)(windowHalf+1);
	
	// create local memory
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

	if(!x->powerSpectrum)
		tIDLib_mag(windowHalf+1, signal_R);

	dividend=1; // to get the product of all terms for geometric mean
	divisor=0;
	flatness=0;
	
	// geometric mean
	// take the nth roots first so as not to lose data.
	for(i=0; i<=windowHalf; i++)
			x->nthRoots[i] = pow(signal_R[i], windowHalfPlusOneRecip);

	// take the product of nth roots
	// what to do with values that are zero?  for now, ignoring them.	
	for(i=0; i<=windowHalf; i++)
		if(x->nthRoots[i] != 0)
			dividend *= x->nthRoots[i];

	for(i=0; i<=windowHalf; i++)
		divisor += signal_R[i];
	
	divisor *= windowHalfPlusOneRecip; // arithmetic mean
	
	if(divisor==0)
		divisor = 1;
	
	flatness = dividend/divisor;
	
	outlet_float(x->x_flatness, flatness);

	// free local memory
	t_freebytes(signal_R, window*sizeof(t_sample));
	t_freebytes(signal_I, (windowHalf+1)*sizeof(t_sample));
}


static void specFlatness_tilde_window(t_specFlatness_tilde *x, t_floatarg w)
{
	int i, window, isPow2;
	
	window = w;
	
	isPow2 = window && !( (window-1) & window );
	
	if( !isPow2 )
		error("requested window size is not a power of 2");
	else
	{
		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, (x->window+x->n) * sizeof(t_sample), (window+x->n) * sizeof(t_sample));
		x->blackman = (t_float *)t_resizebytes(x->blackman, x->window*sizeof(t_float), window*sizeof(t_float));
		x->cosine = (t_float *)t_resizebytes(x->cosine, x->window*sizeof(t_float), window*sizeof(t_float));
		x->hamming = (t_float *)t_resizebytes(x->hamming, x->window*sizeof(t_float), window*sizeof(t_float));
		x->hann = (t_float *)t_resizebytes(x->hann, x->window*sizeof(t_float), window*sizeof(t_float));
		x->nthRoots = (t_float *)t_resizebytes(x->nthRoots, (x->window*0.5+1)*sizeof(t_float), (window*0.5+1)*sizeof(t_float));

		x->window = (t_float)window;

		// re-init window functions
		tIDLib_blackmanWindow(x->blackman, x->window);
		tIDLib_cosineWindow(x->cosine, x->window);
		tIDLib_hammingWindow(x->hamming, x->window);
		tIDLib_hannWindow(x->hann, x->window);

		// init signal buffer
		for(i=0; i<(x->window+x->n); i++)
			x->signal_R[i] = 0.0;
    
		for(i=0; i<=x->window*0.5; i++)
			x->nthRoots[i] = 0.0;
		
		post("window size: %i", (int)x->window);
	}
}


static void specFlatness_tilde_overlap(t_specFlatness_tilde *x, t_floatarg o)
{
	int overlap;

	overlap = o;

	// this change will be picked up in _dsp, where things will be updated based on the samplerate sp[0]->s_sr/x->overlap;
	if(overlap > 0)
		x->overlap = overlap;
	else
		error("overlap must be at least 1.");

    post("overlap: %i", x->overlap);
}


static void specFlatness_tilde_windowFunction(t_specFlatness_tilde *x, t_floatarg f)
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
static void specFlatness_tilde_powerSpectrum(t_specFlatness_tilde *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum");
	else
		post("using magnitude spectrum");
}


static void *specFlatness_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    t_specFlatness_tilde *x = (t_specFlatness_tilde *)pd_new(specFlatness_tilde_class);
	int i, isPow2;
	s=s;
	
	x->x_flatness = outlet_new(&x->x_obj, &s_float);

	if(argc > 0)
	{
		x->window = atom_getfloat(argv);
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );
		
		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead");
			x->window = 1024;
		};
	}
	else
		x->window = 1024;	

	x->sr = 44100.0;
	x->n = 64.0;
	x->overlap = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0;
	x->lastDspTime = clock_getlogicaltime();

	x->signal_R = (t_sample *)t_getbytes((x->window+x->n) * sizeof(t_sample));
	x->nthRoots = (t_float *)t_getbytes((x->window*0.5+1)*sizeof(t_float));

 	for(i=0; i<(x->window+x->n); i++)
		x->signal_R[i] = 0.0;

 	for(i=0; i<=x->window*0.5; i++)
		x->nthRoots[i] = 0.0;
		
  	x->blackman = (t_float *)t_getbytes(x->window*sizeof(t_float));
  	x->cosine = (t_float *)t_getbytes(x->window*sizeof(t_float));
  	x->hamming = (t_float *)t_getbytes(x->window*sizeof(t_float));
  	x->hann = (t_float *)t_getbytes(x->window*sizeof(t_float));

 	// initialize signal windowing functions
	tIDLib_blackmanWindow(x->blackman, x->window);
	tIDLib_cosineWindow(x->cosine, x->window);
	tIDLib_hammingWindow(x->hamming, x->window);
	tIDLib_hannWindow(x->hann, x->window);

    post("specFlatness~: window size: %i", (int)x->window);
    
    return (x);
}


static t_int *specFlatness_tilde_perform(t_int *w)
{
    int i, n;

    t_specFlatness_tilde *x = (t_specFlatness_tilde *)(w[1]);

    t_sample *in = (t_float *)(w[2]);
    n = w[3];
 			
 	// shift signal buffer contents back.
	for(i=0; i<x->window; i++)
		x->signal_R[i] = x->signal_R[i+n];
	
	// write new block to end of signal buffer.
	for(i=0; i<n; i++)
		x->signal_R[(int)x->window+i] = in[i];
		
	x->lastDspTime = clock_getlogicaltime();

    return (w+4);
}


static void specFlatness_tilde_dsp(t_specFlatness_tilde *x, t_signal **sp)
{
	int i;
	
	dsp_add(
		specFlatness_tilde_perform,
		3,
		x,
		sp[0]->s_vec,
		sp[0]->s_n
	); 

// compare n to stored n and recalculate filterbank if different
	if(sp[0]->s_sr != (x->sr*x->overlap) || sp[0]->s_n != x->n)
	{
		x->signal_R = (t_sample *)t_resizebytes(x->signal_R, (x->window+x->n) * sizeof(t_sample), (x->window+sp[0]->s_n) * sizeof(t_sample));

		x->sr = sp[0]->s_sr/x->overlap;
		x->n = sp[0]->s_n;
		x->lastDspTime = clock_getlogicaltime();

		// init signal buffer
		for(i=0; i<(x->window+x->n); i++)
			x->signal_R[i] = 0.0;
    
    	post("specFlatness~: window size: %i. overlap: %i. sampling rate: %i, block size: %i", (int)x->window, x->overlap, (int)x->sr, (int)x->n);
	};
};

static void specFlatness_tilde_free(t_specFlatness_tilde *x)
{
	// free the input buffer memory
    t_freebytes(x->signal_R, (x->window+x->n)*sizeof(t_sample));

	t_freebytes(x->nthRoots, (x->window*0.5+1)*sizeof(t_float));

	// free the window memory
	t_freebytes(x->blackman, x->window*sizeof(t_float));
	t_freebytes(x->cosine, x->window*sizeof(t_float));
	t_freebytes(x->hamming, x->window*sizeof(t_float));
	t_freebytes(x->hann, x->window*sizeof(t_float));
}

void specFlatness_tilde_setup(void)
{
    specFlatness_tilde_class = 
    class_new(
    	gensym("specFlatness~"),
    	(t_newmethod)specFlatness_tilde_new,
    	(t_method)specFlatness_tilde_free,
        sizeof(t_specFlatness_tilde),
        CLASS_DEFAULT, 
        A_GIMME,
		0
    );

    CLASS_MAINSIGNALIN(specFlatness_tilde_class, t_specFlatness_tilde, x_f);

	class_addbang(specFlatness_tilde_class, specFlatness_tilde_bang);

	class_addmethod(
		specFlatness_tilde_class, 
        (t_method)specFlatness_tilde_window,
		gensym("window"),
		A_DEFFLOAT,
		0
	);
	
	class_addmethod(
		specFlatness_tilde_class, 
        (t_method)specFlatness_tilde_overlap,
		gensym("overlap"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlatness_tilde_class,
        (t_method)specFlatness_tilde_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlatness_tilde_class, 
        (t_method)specFlatness_tilde_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);
	
    class_addmethod(
    	specFlatness_tilde_class,
    	(t_method)specFlatness_tilde_dsp,
    	gensym("dsp"),
    	0
    );
}