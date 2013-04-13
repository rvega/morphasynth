/*

specBrightness~

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.2.5, December 21, 2011

¥ 0.2.5 incorporating tIDLib.h for timbreID-0.6.0 release
¥ 0.2.4 as part of timbreID-0.5 update, getting rid of unnecessary getbytes(0) calls. also adding power spectrum option. completely removing normalization functions (useless for this relative feature).
¥Ê0.2.3 added an ifndef M_PI for guaranteed windows compilation
¥ 0.2.2 adds a #define M_PI for windows compilation, and declares all functions except _setup static
¥ 0.2.1 is part of the update that ensures all function names are prepended by the external name (bfcc_ or bfcc_tilde_, etc).
¥ 0.2.0 implements mayer_realfft
¥ÊCommenting out normalization since it's basically irrelevant in ratio measures lke this.
¥ 0.1.8 added normalization option

*/

#include "tIDLib.h"

static t_class *specBrightness_tilde_class;

typedef struct _specBrightness_tilde
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
    t_float *binFreqs;
    t_float freqBoundary;
    int binBoundary;
    t_float *blackman;
    t_float *cosine;
    t_float *hamming;
    t_float *hann;
    t_outlet *x_brightness;
    t_float x_f;

} t_specBrightness_tilde;


/* ------------------------ specBrightness~ -------------------------------- */

static void specBrightness_tilde_bang(t_specBrightness_tilde *x)
{
    int i, j, window, windowHalf, bangSample;
    t_sample *signal_R, *signal_I;
    t_float dividend, divisor, brightness, *windowFuncPtr;
	double currentTime;

    window = x->window;
    windowHalf = window*0.5;
	
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

	dividend=0;
	divisor=0;
	brightness=0;
	
	for(i=x->binBoundary; i<=windowHalf; i++)
		dividend += signal_R[i];

	for(i=0; i<=windowHalf; i++)
		divisor += signal_R[i];
	
	divisor = (divisor==0)?1:divisor;
		
	brightness = dividend/divisor;
		
	outlet_float(x->x_brightness, brightness);

	// free local memory
	t_freebytes(signal_R, window*sizeof(t_sample));
	t_freebytes(signal_I, (windowHalf+1)*sizeof(t_sample));
}


static void specBrightness_tilde_boundary(t_specBrightness_tilde *x, t_floatarg b)
{
	if(b<0 || b>(x->sr*0.5))
		error("boundary frequency must be a positive real number and less than Nyquist.");
	else
		x->freqBoundary = b;

    x->binBoundary = tIDLib_nearestBinIndex(x->freqBoundary, x->binFreqs, x->window);
    
    post("boundary frequency: %f", x->freqBoundary);

}


static void specBrightness_tilde_window(t_specBrightness_tilde *x, t_floatarg w)
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
		x->binFreqs = (t_float *)t_resizebytes(x->binFreqs, x->window*sizeof(t_float), window*sizeof(t_float));

		x->window = (t_float)window;

		// re-init window functions
		tIDLib_blackmanWindow(x->blackman, x->window);
		tIDLib_cosineWindow(x->cosine, x->window);
		tIDLib_hammingWindow(x->hamming, x->window);
		tIDLib_hannWindow(x->hann, x->window);

		// init signal buffer
		for(i=0; i<(x->window+x->n); i++)
			x->signal_R[i] = 0.0;
	
		// freqs for each bin based on current window size and sample rate
		for(i=0; i<x->window; i++)
			x->binFreqs[i] = (x->sr/x->window)*i;
			
		x->binBoundary = tIDLib_nearestBinIndex(x->freqBoundary, x->binFreqs, x->window);
    
		post("window size: %i", (int)x->window);
	}
}


static void specBrightness_tilde_overlap(t_specBrightness_tilde *x, t_floatarg o)
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


static void specBrightness_tilde_windowFunction(t_specBrightness_tilde *x, t_floatarg f)
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
static void specBrightness_tilde_powerSpectrum(t_specBrightness_tilde *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum");
	else
		post("using magnitude spectrum");
}


static void *specBrightness_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    t_specBrightness_tilde *x = (t_specBrightness_tilde *)pd_new(specBrightness_tilde_class);
	int i, isPow2;
	s=s;
	
	x->x_brightness = outlet_new(&x->x_obj, &s_float);

	x->sr = 44100.0;
	x->n = 64.0;
	
	if(argc > 1)
	{
		x->window = atom_getfloat(argv);
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );
		
		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead");
			x->window = 1024;
		};

		x->freqBoundary = atom_getfloat(argv+1);
		if(x->freqBoundary<0)
		{
			error("boundary frequency must be a positive real number and less than Nyquist.");
			x->freqBoundary = 1200;
		}
		else if(x->freqBoundary>(x->sr*0.5))
		{
			error("boundary frequency must be a positive real number and less than Nyquist.");
			x->freqBoundary = 1200;
		};
		
	}
	else if(argc > 0)
	{
		x->window = atom_getfloat(argv);
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );
		
		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead");
			x->window = 1024;
		};
		
		x->freqBoundary = 1200;

	}
	else
	{
		x->window = 1024;
		x->freqBoundary = 1200;
	};

	x->overlap = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0;
	x->lastDspTime = clock_getlogicaltime();

	x->signal_R = (t_sample *)t_getbytes((x->window+x->n) * sizeof(t_sample));

 	for(i=0; i<(x->window+x->n); i++)
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

	x->binFreqs = (t_float *)t_getbytes(x->window*sizeof(t_float));

	// freqs for each bin based on current window size and sample rate
	for(i=0; i<x->window; i++)
		x->binFreqs[i] = (x->sr/x->window)*i;
		
    x->binBoundary = tIDLib_nearestBinIndex(x->freqBoundary, x->binFreqs, x->window);

    post("specBrightness~: window size: %i", (int)x->window);
    
    return (x);
}


static t_int *specBrightness_tilde_perform(t_int *w)
{
    int i, n;

    t_specBrightness_tilde *x = (t_specBrightness_tilde *)(w[1]);

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


static void specBrightness_tilde_dsp(t_specBrightness_tilde *x, t_signal **sp)
{
	int i;
	
	dsp_add(
		specBrightness_tilde_perform,
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

		// freqs for each bin based on current window size and sample rate
		for(i=0; i<x->window; i++)
			x->binFreqs[i] = (x->sr/x->window)*i;
			
		x->binBoundary = tIDLib_nearestBinIndex(x->freqBoundary, x->binFreqs, x->window);
    
    	post("specBrightness~: window size: %i. overlap: %i. sampling rate: %i, block size: %i", (int)x->window, x->overlap, (int)x->sr, (int)x->n);
	};
};

static void specBrightness_tilde_free(t_specBrightness_tilde *x)
{
	// free the input buffer memory
    t_freebytes(x->signal_R, (x->window+x->n)*sizeof(t_sample));

	// free the binFreqs memory
    t_freebytes(x->binFreqs, x->window*sizeof(t_sample));

	// free the window memory
	t_freebytes(x->blackman, x->window*sizeof(t_float));
	t_freebytes(x->cosine, x->window*sizeof(t_float));
	t_freebytes(x->hamming, x->window*sizeof(t_float));
	t_freebytes(x->hann, x->window*sizeof(t_float));
}

void specBrightness_tilde_setup(void)
{
    specBrightness_tilde_class = 
    class_new(
    	gensym("specBrightness~"),
    	(t_newmethod)specBrightness_tilde_new,
    	(t_method)specBrightness_tilde_free,
        sizeof(t_specBrightness_tilde),
        CLASS_DEFAULT, 
        A_GIMME,
		0
    );

    CLASS_MAINSIGNALIN(specBrightness_tilde_class, t_specBrightness_tilde, x_f);

	class_addbang(specBrightness_tilde_class, specBrightness_tilde_bang);

	class_addmethod(
		specBrightness_tilde_class, 
        (t_method)specBrightness_tilde_boundary,
		gensym("boundary"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specBrightness_tilde_class, 
        (t_method)specBrightness_tilde_window,
		gensym("window"),
		A_DEFFLOAT,
		0
	);
	
	class_addmethod(
		specBrightness_tilde_class, 
        (t_method)specBrightness_tilde_overlap,
		gensym("overlap"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specBrightness_tilde_class,
        (t_method)specBrightness_tilde_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specBrightness_tilde_class, 
        (t_method)specBrightness_tilde_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);
	
    class_addmethod(
    	specBrightness_tilde_class,
    	(t_method)specBrightness_tilde_dsp,
    	gensym("dsp"),
    	0
    );
}