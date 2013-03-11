/*

specRolloff~

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.2.6, December 22, 2011

¥ 0.2.6 incorporating tIDLib.h for timbreID-0.6.0 release
¥ 0.2.5 as part of timbreID-0.5 update, getting rid of unnecessary getbytes(0) calls. also adding power spectrum option. completely removing normalization functions (useless for this relative feature).
¥Ê0.2.4 added an ifndef M_PI for guaranteed windows compilation
¥ 0.2.3 adds a #define M_PI for windows compilation, and declares all functions except _setup static
¥ 0.2.2 adds a break out of the concentration while loop in _analyze. Maybe because of underflow, sometimes energy never got up to its target.
¥Ê0.2.1 bang function had a memory bug that caused a segmentation fault: first arg to _abs function was window rather than window_half
¥ 0.2.0 implements mayer_realfft
¥ÊCommenting out normalization since it's basically irrelevant in ratio measures lke this.
¥ 0.1.8 added normalization option

*/

#include "tIDLib.h"

static t_class *specRolloff_tilde_class;

typedef struct _specRolloff_tilde
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
    t_float concentration;
    t_float *blackman;
    t_float *cosine;
    t_float *hamming;
    t_float *hann;
    t_outlet *x_rolloff;
    t_float x_f;

} t_specRolloff_tilde;


/* ------------------------ specRolloff~ -------------------------------- */

static void specRolloff_tilde_bang(t_specRolloff_tilde *x)
{
    int i, j, window, windowHalf, bangSample;
    t_sample *signal_R, *signal_I;
    t_float energy, energyTarget, rolloff, *windowFuncPtr;
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

	energyTarget=0;

	for(i=0; i<=windowHalf; i++)
		energyTarget += signal_R[i];

	energyTarget *= x->concentration;

	energy=0;
	i=0;

	while(energy <= energyTarget)
	{
		energy += signal_R[i];
		i++;

		if(i>windowHalf)
			break;
	}

	i = (i==0)?1:i;

	rolloff = x->binFreqs[i-1]; // back up one because the last one went over...

	outlet_float(x->x_rolloff, rolloff);

	// free local memory
	t_freebytes(signal_R, window*sizeof(t_sample));
	t_freebytes(signal_I, (windowHalf+1)*sizeof(t_sample));
}


static void specRolloff_tilde_concentration(t_specRolloff_tilde *x, t_floatarg c)
{
	if(c<0 || c>1.0)
		error("concentration must be between 0.0 and 1.0.");
	else
		x->concentration = c;

    post("concentration: %f", x->concentration);

}


static void specRolloff_tilde_window(t_specRolloff_tilde *x, t_floatarg w)
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

		post("window size: %i", (int)x->window);
	}
}


static void specRolloff_tilde_overlap(t_specRolloff_tilde *x, t_floatarg o)
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


static void specRolloff_tilde_windowFunction(t_specRolloff_tilde *x, t_floatarg f)
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
static void specRolloff_tilde_powerSpectrum(t_specRolloff_tilde *x, t_floatarg spec)
{
    spec = (spec<0)?0:spec;
    spec = (spec>1)?1:spec;
	x->powerSpectrum = spec;

	if(x->powerSpectrum)
		post("using power spectrum");
	else
		post("using magnitude spectrum");
}


static void *specRolloff_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    t_specRolloff_tilde *x = (t_specRolloff_tilde *)pd_new(specRolloff_tilde_class);
	int i, isPow2;
	s=s;

	x->x_rolloff = outlet_new(&x->x_obj, &s_float);

	if(argc > 1)
	{
		x->window = atom_getfloat(argv);
		isPow2 = (int)x->window && !( ((int)x->window-1) & (int)x->window );

		if(!isPow2)
		{
			error("requested window size is not a power of 2. default value of 1024 used instead");
			x->window = 1024;
		};

		x->concentration = atom_getfloat(argv+1);
		if(x->concentration<0)
		{
			error("concentration must be between 0.0 and 1.0.");
			x->concentration = 0.85;
		}
		else if(x->concentration>1.0)
		{
			error("concentration must be between 0.0 and 1.0.");
			x->concentration = 0.85;
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

		x->concentration = 0.85;

	}
	else
	{
		x->window = 1024;
		x->concentration = 0.85;
	};

	x->sr = 44100.0;
	x->n = 64.0;
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

    post("specRolloff~: window size: %i", (int)x->window);

    return (x);
}


static t_int *specRolloff_tilde_perform(t_int *w)
{
    int i, n;

    t_specRolloff_tilde *x = (t_specRolloff_tilde *)(w[1]);

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


static void specRolloff_tilde_dsp(t_specRolloff_tilde *x, t_signal **sp)
{
	int i;

	dsp_add(
		specRolloff_tilde_perform,
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

    	post("specRolloff~: window size: %i. overlap: %i. sampling rate: %i, block size: %i", (int)x->window, x->overlap, (int)x->sr, (int)x->n);
	};
};

static void specRolloff_tilde_free(t_specRolloff_tilde *x)
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

void specRolloff_tilde_setup(void)
{
    specRolloff_tilde_class =
    class_new(
    	gensym("specRolloff~"),
    	(t_newmethod)specRolloff_tilde_new,
    	(t_method)specRolloff_tilde_free,
        sizeof(t_specRolloff_tilde),
        CLASS_DEFAULT,
        A_GIMME,
		0
    );

    CLASS_MAINSIGNALIN(specRolloff_tilde_class, t_specRolloff_tilde, x_f);

	class_addbang(specRolloff_tilde_class, specRolloff_tilde_bang);

	class_addmethod(
		specRolloff_tilde_class,
        (t_method)specRolloff_tilde_concentration,
		gensym("concentration"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_tilde_class,
        (t_method)specRolloff_tilde_window,
		gensym("window"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_tilde_class,
        (t_method)specRolloff_tilde_overlap,
		gensym("overlap"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_tilde_class,
        (t_method)specRolloff_tilde_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		specRolloff_tilde_class,
        (t_method)specRolloff_tilde_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);

    class_addmethod(
    	specRolloff_tilde_class,
    	(t_method)specRolloff_tilde_dsp,
    	gensym("dsp"),
    	0
    );
}

