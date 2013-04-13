/*

specFlux~

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.2.4, December 22, 2011

¥ 0.2.4 incorporating tIDLib.h for timbreID-0.6.0 release
¥ 0.2.3 as part of timbreID-0.5 update, getting rid of unnecessary getbytes(0) calls. also adding power spectrum option. completely removing normalization functions (useless for this relative feature).
¥Ê0.2.2 added an ifndef M_PI for guaranteed windows compilation
¥ 0.2.1 implements mayer_realfft
¥ 0.2.0 making squared difference vs. absolute value of difference an option. Also making a second outlet, so that it reports the full list of squared differences out the left, and the sum of the squared differences at the right.
¥ 0.1.9 fixed bug: normalization infrastructure was there, but it wasn't actually implemented in the _bang function! Also, trying fabs(diff) instead of squared difference to see if a larger number will be more useful.
¥ 0.1.8 added normalization option

*/

#include "tIDLib.h"

static t_class *specFlux_tilde_class;

typedef struct _specFlux_tilde
{
    t_object x_obj;
    t_float sr;
    t_float n;
	int windowFunction;
    int overlap;
    int powerSpectrum;
    int normalize;
    int window;
    int squaredDiff;
	int separation;
    double lastDspTime;
    t_sample *signal_R;
    t_float *blackman;
    t_float *cosine;
    t_float *hamming;
    t_float *hann;
    t_outlet *x_fluxList;
    t_outlet *x_flux;
    t_float x_f;

} t_specFlux_tilde;


/* ------------------------ specFlux~ -------------------------------- */

static void specFlux_tilde_bang(t_specFlux_tilde *x)
{
    int i, j, window, windowHalf, bangSample;
    t_float flux, *windowFuncPtr;
    t_atom *listOut;
    t_sample *signal2_R, *signal2_I, *signal1_R, *signal1_I;
	double currentTime;

    window = x->window;
    windowHalf = window*0.5;

	if(x->separation>window)
		x->separation = window*0.25;

	// create local memory
	listOut = (t_atom *)t_getbytes((windowHalf+1) * sizeof(t_atom));
	signal2_R = (t_sample *)t_getbytes(window*sizeof(t_sample));
	signal2_I = (t_sample *)t_getbytes((windowHalf+1)*sizeof(t_sample));
	signal1_R = (t_sample *)t_getbytes(window*sizeof(t_sample));
	signal1_I = (t_sample *)t_getbytes((windowHalf+1)*sizeof(t_sample));
		
	currentTime = clock_gettimesince(x->lastDspTime);
	bangSample = (int)(((currentTime/1000.0)*x->sr)+0.5); // round

	if (bangSample < 0)
        bangSample = 0;
    else if ( bangSample >= x->n )
        bangSample = x->n - 1;
            
	// construct newest analysis window
	for(i=0, j=bangSample; i<window; i++, j++)
		signal2_R[i] = x->signal_R[window + j];

	// construct older analysis window x->separation frames earlier
	for(i=0, j=bangSample; i<window; i++, j++)
		signal1_R[i] = x->signal_R[window - x->separation + j];
	
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
			signal1_R[i] *= *windowFuncPtr;

	mayer_realfft(window, signal1_R);
	tIDLib_realfftUnpack(window, windowHalf, signal1_R, signal1_I);
	tIDLib_power(windowHalf+1, signal1_R, signal1_I);

	if(!x->powerSpectrum)
		tIDLib_mag(windowHalf+1, signal1_R);
		
	if(x->normalize)
		tIDLib_normal(windowHalf+1, signal1_R);

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
			signal2_R[i] *= *windowFuncPtr;

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
	t_freebytes(listOut, (windowHalf+1)*sizeof(t_atom));
	t_freebytes(signal1_R, window*sizeof(t_sample));
	t_freebytes(signal1_I, (windowHalf+1)*sizeof(t_sample));
	t_freebytes(signal2_R, window*sizeof(t_sample));
	t_freebytes(signal2_I, (windowHalf+1)*sizeof(t_sample));
}


static void specFlux_tilde_window(t_specFlux_tilde *x, t_floatarg w)
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

		x->window = (t_float)window;

		// re-init window functions
		tIDLib_blackmanWindow(x->blackman, x->window);
		tIDLib_cosineWindow(x->cosine, x->window);
		tIDLib_hammingWindow(x->hamming, x->window);
		tIDLib_hannWindow(x->hann, x->window);

		// init signal buffer
		for(i=0; i<(x->window+x->n); i++)
			x->signal_R[i] = 0.0;
    
		post("window size: %i", (int)x->window);
	}
}


static void specFlux_tilde_overlap(t_specFlux_tilde *x, t_floatarg o)
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


static void specFlux_tilde_windowFunction(t_specFlux_tilde *x, t_floatarg f)
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
static void specFlux_tilde_powerSpectrum(t_specFlux_tilde *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum");
	else
		post("using magnitude spectrum");
}


void specFlux_tilde_squaredDiff(t_specFlux_tilde *x, t_floatarg sd)
{
    sd = (sd<0)?0:sd;
    sd = (sd>1)?1:sd;
	x->squaredDiff = sd;
	
	if(x->squaredDiff)
		post("difference**2");
	else
		post("|difference|");
}


static void specFlux_tilde_normalize(t_specFlux_tilde *x, t_floatarg norm)
{
    norm = (norm<0)?0:norm;
    norm = (norm>1)?1:norm;
	x->normalize = norm;

	if(x->normalize)
		post("spectrum normalization ON.");
	else
		post("spectrum normalization OFF.");
}


void specFlux_tilde_separation(t_specFlux_tilde *x, t_floatarg s)
{
	x->separation = s;

	if(x->separation > x->window)
	{
		error("analysis windows cannot be more than %i frames apart", x->window);
		x->separation = 128;
	}
	else if(x->separation < 0)
	{
		error("frame separation must be > 0");
		x->separation = 128;
	};
		
    post("frame separation: %i", x->separation);

}


static void *specFlux_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    t_specFlux_tilde *x = (t_specFlux_tilde *)pd_new(specFlux_tilde_class);
	int i, isPow2;
	s=s;
	
	x->x_flux = outlet_new(&x->x_obj, &s_float);
	x->x_fluxList = outlet_new(&x->x_obj, gensym("list"));

	if(argc > 1)
	{
		x->window = atom_getfloat(argv);
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );
		
		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead");
			x->window = 1024;
		};
		
		x->separation = atom_getfloat(argv+1);
		
		if(x->separation > x->window)
		{
			error("analysis windows cannot be more than %i frames apart", x->window);
			x->separation = x->window;
		}
		else if(x->separation < 0)
		{
			error("frame separation must be > 0");
			x->separation = x->window*0.25;
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
		
		x->separation = x->window*0.25;
	}
	else
	{
		x->window = 1024;
		x->separation = x->window*0.25;
	}

	x->sr = 44100.0;
	x->n = 64.0;
	x->overlap = 1;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 0;
	x->lastDspTime = clock_getlogicaltime();
	x->squaredDiff = 0; // absolute value by default
	x->normalize = 1;

	x->signal_R = (t_sample *)t_getbytes((x->window*2+x->n) * sizeof(t_sample));

 	for(i=0; i<(x->window*2+x->n); i++)
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

    post("specFlux~: window size: %i", (int)x->window);
    
    return (x);
}


static t_int *specFlux_tilde_perform(t_int *w)
{
    int i, n;

    t_specFlux_tilde *x = (t_specFlux_tilde *)(w[1]);

    t_sample *in = (t_float *)(w[2]);
    n = w[3];
 			
 	// shift signal buffer contents back.
	for(i=0; i<(x->window*2); i++)
		x->signal_R[i] = x->signal_R[i+n];
	
	// write new block to end of signal buffer.
	for(i=0; i<n; i++)
		x->signal_R[(int)(x->window*2)+i] = in[i];
		
	x->lastDspTime = clock_getlogicaltime();

    return (w+4);
}


static void specFlux_tilde_dsp(t_specFlux_tilde *x, t_signal **sp)
{
	int i;
	
	dsp_add(
		specFlux_tilde_perform,
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
    
    	post("specFlux~: window size: %i. overlap: %i. sampling rate: %i, block size: %i", (int)x->window, x->overlap, (int)x->sr, (int)x->n);
	};
};

static void specFlux_tilde_free(t_specFlux_tilde *x)
{
	// free the input buffer memory
    t_freebytes(x->signal_R, (x->window*2+x->n)*sizeof(t_sample));

	// free the window memory
	t_freebytes(x->blackman, x->window*sizeof(t_float));
	t_freebytes(x->cosine, x->window*sizeof(t_float));
	t_freebytes(x->hamming, x->window*sizeof(t_float));
	t_freebytes(x->hann, x->window*sizeof(t_float));
}

void specFlux_tilde_setup(void)
{
    specFlux_tilde_class = 
    class_new(
    	gensym("specFlux~"),
    	(t_newmethod)specFlux_tilde_new,
    	(t_method)specFlux_tilde_free,
        sizeof(t_specFlux_tilde),
        CLASS_DEFAULT, 
        A_GIMME,
		0
    );

    CLASS_MAINSIGNALIN(specFlux_tilde_class, t_specFlux_tilde, x_f);

	class_addbang(specFlux_tilde_class, specFlux_tilde_bang);

	class_addmethod(
		specFlux_tilde_class, 
        (t_method)specFlux_tilde_window,
		gensym("window"),
		A_DEFFLOAT,
		0
	);
	
	class_addmethod(
		specFlux_tilde_class, 
        (t_method)specFlux_tilde_separation,
		gensym("separation"),
		A_DEFFLOAT,
		0
	);
	
	class_addmethod(
		specFlux_tilde_class, 
        (t_method)specFlux_tilde_overlap,
		gensym("overlap"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_tilde_class, 
        (t_method)specFlux_tilde_squaredDiff, 
		gensym("squared_diff"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_tilde_class, 
        (t_method)specFlux_tilde_normalize, 
		gensym("normalize"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_tilde_class,
        (t_method)specFlux_tilde_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specFlux_tilde_class, 
        (t_method)specFlux_tilde_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);
	
    class_addmethod(
    	specFlux_tilde_class,
    	(t_method)specFlux_tilde_dsp,
    	gensym("dsp"),
    	0
    );
}