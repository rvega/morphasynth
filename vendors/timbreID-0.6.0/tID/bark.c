/*

bark - A non-real-time Bark-frequency spectrum based attack detector.

Copyright 2010 William Brent

bark is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

bark is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.0.6, December 20, 2011

¥ 0.0.6 incorporates tIDLib for inclusion with timbreID-0.6.0
¥ 0.0.5 analyze message now takes arguments for start and stop time in seconds. if times aren't specified, it just analyzes the whole thing. also changed attack time outlet to give time in seconds rather than samples.  that's more convenient for labels
¥ 0.0.4 added samplerate and filterbank functions.  moved create_loudness_weights to the end of create_filterbank so that it's done whenever the bank changes.
¥ 0.0.3 added a half window of samples as a correction. added spew mode. set x->mask to zero so that we don't have old mask info on the 2nd, 3rd runs etc.
¥ 0.0.2 adds creation arguments for window size and hop.  next: make the analyze message have arguments for start and stop time in seconds(?).  what about spew mode to look at growth patterns?  could address the fact that this version reports attacks half a window EARLY while the RT version reports LATE. would just need to start at frame=1 instead of frame=0, and fill the signal buffer from that point backwards. then the two versions would be consistent.

¥ based on bark~ version 0.1.7

*/

#include "tIDLib.h"

#define NUMWEIGHTPOINTS 29
#define BLOCKSIZE 64

t_float weights_dB[] = {-69.9, -60.4, -51.4, -43.3, -36.6, -30.3, -24.3, -19.5, -14.8, -10.7, -7.5, -4.8, -2.6, -0.8, 0.0, 0.6, 0.5, 0.0, -0.1, 0.5, 1.5, 3.6, 5.9, 6.5, 4.2, -2.6, -10.2, -10.0, -2.8};

t_float weights_freqs[] = {20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500};

static t_class *bark_class;

typedef struct _bark
{
    t_object x_obj;
    int debug;
    t_float sr;
    t_float n;

	t_word *x_vec;
	t_symbol *x_arrayname;
	int x_array_points;

	t_float window;
    int hop;

    int windowFunction;
	int powerSpectrum;
	int normalize;
	int filterAvg;
	int useWeights;
	int spew;

    t_float barkSpacing;
    int numFilters;
    int sizeFilterFreqs;
    t_float *x_filterFreqs;
    t_filter *x_filterbank;
	t_float *loudWeights;

	t_float prevTotalGrowth;
    t_float loThresh;
    t_float hiThresh;
    t_float minvel;
    int loBin;
    int hiBin;
    t_float debounceTime;
    int debounceSamp;
    t_float maskDecay;
    int maskPeriods;
    int *numPeriods;

    int debounceActive;
    int haveHit;

	t_float weights_dB[NUMWEIGHTPOINTS];
	t_float weights_freqs[NUMWEIGHTPOINTS];
    t_float *blackman;
    t_float *cosine;
    t_float *hamming;
    t_float *hann;
	t_sample *signalBuf;
	t_float *analysisBuf;
    t_float *mask;
    t_float *growth;
	t_atom *growthList;

    t_outlet *x_outputList;
    t_outlet *x_growthOut;
    t_outlet *x_timeOut;

} t_bark;


/* ---------------- utility functions ---------------------- */

static void bark_create_loudness_weighting(t_bark *x)
{
	int i;
	t_float bark_sum, *bark_freqs;

	bark_freqs = (t_float *)t_getbytes(x->numFilters * sizeof(t_float));

	bark_sum = x->barkSpacing;

	for(i=0; i<x->numFilters; i++)
	{
		bark_freqs[i] = tIDLib_bark2freq(bark_sum);
		bark_sum += x->barkSpacing;
	}

	for(i=0; i<x->numFilters; i++)
	{
		int nearIdx;
		t_float nearFreq, diffFreq, diffdB, dBint;

		nearIdx = tIDLib_nearestBinIndex(bark_freqs[i], weights_freqs, NUMWEIGHTPOINTS);
		nearFreq = weights_freqs[nearIdx];
		diffdB = 0;

		// this doesn't have to be if/else'd into a greater/less situation.  later on i should write a more general interpolation solution, and maybe move it up to 4 points instead.
		if(bark_freqs[i]>nearFreq)
		{
			if(nearIdx<=NUMWEIGHTPOINTS-2)
			{
				diffFreq = (bark_freqs[i] - nearFreq)/(weights_freqs[nearIdx+1] - nearFreq);
				diffdB = diffFreq * (weights_dB[nearIdx+1] - weights_dB[nearIdx]);
			}

			dBint = weights_dB[nearIdx] + diffdB;
		}
		else
		{
			if(nearIdx>0)
			{
				diffFreq = (bark_freqs[i] - weights_freqs[nearIdx-1])/(nearFreq - weights_freqs[nearIdx-1]);
				diffdB = diffFreq * (weights_dB[nearIdx] - weights_dB[nearIdx-1]);
			}

			dBint = weights_dB[nearIdx-1] + diffdB;
		}

		if(x->powerSpectrum)
			x->loudWeights[i] = pow(10.0, dBint*0.1);
		else
			x->loudWeights[i] = pow(10.0, dBint*0.05);

	}
}

/* ---------------- END utility functions ---------------------- */


/* ------------------------ bark -------------------------------- */

static void bark_windowFunction(t_bark *x, t_floatarg f)
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

static void bark_thresh(t_bark *x, t_floatarg lo, t_floatarg hi)
{
	if(hi<lo)
	{
		post("bark: warning: high threshold less than low threshold.");
		x->hiThresh = lo;
		x->loThresh = hi;

		x->loThresh = (x->loThresh<0)?-1:x->loThresh;

//		post("bark: low thresh: %f, high thresh: %f", x->loThresh, x->hiThresh);
	}
	else
	{
		x->hiThresh = hi;
		x->loThresh = lo;

		x->loThresh = (x->loThresh<0) ?-1:x->loThresh;

//		post("bark: low thresh: %0.2f, high thresh: %0.2f", x->loThresh, x->hiThresh);
	}
}

static void bark_minvel(t_bark *x, t_floatarg mv)
{
	x->minvel = (mv<0)?0:mv;
}

static void bark_filter_range(t_bark *x, t_floatarg lo, t_floatarg hi)
{
	if(hi<lo)
	{
		post("bark: warning: high bin less than low bin.");
		x->hiBin = lo;
		x->loBin = hi;

		x->loBin = (x->loBin<0)?0:x->loBin;
		x->hiBin = (x->hiBin>=x->numFilters)?x->numFilters-1:x->hiBin;

//		post("bark: using filters %i through %i (inclusive) for the growth sum.", x->loBin, x->hiBin);
	}
	else
	{
		x->hiBin = hi;
		x->loBin = lo;

		x->loBin = (x->loBin<0)?0:x->loBin;
		x->hiBin = (x->hiBin>=x->numFilters)?x->numFilters-1:x->hiBin;

//		post("bark: using filters %i through %i (inclusive) for the growth sum.", x->loBin, x->hiBin);
	}
}

static void bark_mask(t_bark *x, t_floatarg per, t_floatarg dec)
{
	x->maskPeriods = per;
	x->maskDecay = dec;

	x->maskPeriods = (x->maskPeriods<0)?0:x->maskPeriods;
	x->maskDecay = (x->maskDecay<0.05)?0.05:x->maskDecay;
	x->maskDecay = (x->maskDecay>0.95)?0.95:x->maskDecay;

//	post("bark: masking for %i periods and decaying by %i%% thereafter.", x->maskPeriods, (int)(x->maskDecay*100));
}

static void bark_debounce(t_bark *x, t_floatarg db)
{
	if(x->debounceTime>=0)
	{
		x->debounceTime = db;
		x->debounceSamp = x->debounceTime*0.001*x->sr;
		post("debounce time: %0.2f, debounce samp: %i", x->debounceTime, x->debounceSamp);
	}
	else
		error("bark: debounce time must be >= 0");
}

static void bark_spew(t_bark *x, t_floatarg spew)
{
	spew = (spew<0)?0:spew;
	spew = (spew>1)?1:spew;
	x->spew = spew;

	post("spew mode: %i", x->spew);
}

static void bark_print(t_bark *x)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for bark", x->x_arrayname->s_name);
	else
		post("total frames: %i", (int)floor((x->x_array_points - x->window)/x->hop));

	post("window size: %i", (int)x->window);
	post("hop: %i", x->hop);
	post("Bark spacing: %0.2f", x->barkSpacing);
	post("no. of filters: %i", x->numFilters);
	post("bin range: %i through %i (inclusive)", x->loBin, x->hiBin);
	post("low thresh: %0.2f, high thresh: %0.2f", x->loThresh, x->hiThresh);
	post("minvel: %f", x->minvel);
	post("mask periods: %i, mask decay: %0.2f", x->maskPeriods, x->maskDecay);
	post("debounce time: %0.2f", x->debounceTime);
	post("normalization: %i", x->normalize);
	post("filter averaging: %i", x->filterAvg);
	post("power spectrum: %i", x->powerSpectrum);
	post("loudness weights: %i", x->useWeights);
	post("spew mode: %i", x->spew);
	post("debug mode: %i", x->debug);
}

static void bark_debug(t_bark *x, t_floatarg debug)
{
	debug = (debug<0)?0:debug;
	debug = (debug>1)?1:debug;
	x->debug = debug;

	if(x->debug)
		post("debug mode ON");
	else
		post("debug mode OFF");
}

static void bark_use_weights(t_bark *x, t_floatarg w)
{
	w = (w<0)?0:w;
	w = (w>1)?1:w;
	x->useWeights = w;

	if(x->useWeights)
		post("using loudness weighting");
	else
		post("using unweighted spectrum");
}


static void bark_hat(t_bark *x, t_floatarg filt)
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


static void bark_filterFreqs(t_bark *x)
{
	int i;

	for(i=1; i<=x->numFilters; i++)
		post("filterFreq[%i]: %f", i-1, x->x_filterFreqs[i]);
}


static void bark_filter_avg(t_bark *x, t_floatarg avg)
{
	avg = (avg<0) ? 0 : avg;
	avg = (avg>1) ? 1 : avg;
	x->filterAvg = avg;

	if(x->filterAvg)
		post("averaging energy in filters.");
	else
		post("summing energy in filters.");
}


// magnitude spectrum == 0, power spectrum == 1
static void bark_powerSpectrum(t_bark *x, t_floatarg spec)
{
	spec = (spec<0) ? 0 : spec;
	spec = (spec>1) ? 1 : spec;
	x->powerSpectrum = spec;

	bark_create_loudness_weighting(x);

	if(x->powerSpectrum)
		post("using power spectrum.");
	else
		post("using magnitude spectrum.");
}


static void bark_normalize(t_bark *x, t_floatarg norm)
{
	norm = (norm<0) ? 0 : norm;
	norm = (norm>1) ? 1 : norm;
	x->normalize = norm;

	if(x->normalize)
		post("spectrum normalization ON.");
	else
		post("spectrum normalization OFF.");
}

static void bark_samplerate(t_bark *x, t_floatarg sr)
{
    int i, oldNumFilters;

    oldNumFilters = x->numFilters;

	if(sr<=0)
	{
		error("samplerate must be > 0. default value of 44100 used instead.");
		x->sr = 44100;
	}
	else
		x->sr = sr;

	x->debounceSamp = x->debounceTime*0.001*x->sr;

    x->numFilters = tIDLib_getBarkFilterSpacing(&x->x_filterFreqs, x->sizeFilterFreqs, x->barkSpacing, x->sr);

    x->sizeFilterFreqs = x->numFilters+2;

    tIDLib_createFilterbank(x->x_filterFreqs, &x->x_filterbank, oldNumFilters, x->numFilters, x->window, x->sr);

    x->loBin = 0;
    x->hiBin = x->numFilters-1;

    x->mask = (t_float *)t_resizebytes(x->mask, oldNumFilters*sizeof(t_float), x->numFilters*sizeof(t_float));
    x->growth = (t_float *)t_resizebytes(x->growth, oldNumFilters*sizeof(t_float), x->numFilters*sizeof(t_float));
    x->numPeriods = (int *)t_resizebytes(x->numPeriods, oldNumFilters*sizeof(int), x->numFilters*sizeof(int));
    x->growthList = (t_atom *)t_resizebytes(x->growthList, oldNumFilters*sizeof(t_atom), x->numFilters*sizeof(t_atom));
    x->loudWeights = (t_float *)t_resizebytes(x->loudWeights, oldNumFilters*sizeof(t_float), x->numFilters*sizeof(t_float));

    for(i=0; i<x->numFilters; i++)
    {
	    x->mask[i] = 0.0;
	    x->growth[i] = 0.0;
	    x->numPeriods[i] = 0.0;
	    SETFLOAT(x->growthList+i, 0.0);
	    x->loudWeights[i] = 0.0;
    }
}

static void *bark_new(t_symbol *s, t_floatarg w, t_floatarg h, t_floatarg bs)
{
    t_bark *x = (t_bark *)pd_new(bark_class);
	int i, isPow2;
	t_garray *a;

	x->x_timeOut = outlet_new(&x->x_obj, &s_float);
	x->x_growthOut = outlet_new(&x->x_obj, &s_float);
	x->x_outputList = outlet_new(&x->x_obj, gensym("list"));

	if(bs)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
	    	pd_error(x, "%s: bad template for bark", x->x_arrayname->s_name);

		isPow2 = (int)w && !( ((int)w-1) & (int)w );

		if( !isPow2 )
		{
			error("requested window size is not a power of 2. using default value of 2048 instead.");
			x->window = 2048;
		}
		else
			x->window = w;

		if(h<0)
		{
			error("hop value must be > 0. default value of 128 samples used instead.");
			x->hop = 128;
		}
		else
			x->hop = h;

		x->barkSpacing = bs;
	}
	else if(h)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
	    	pd_error(x, "%s: bad template for bark", x->x_arrayname->s_name);

		isPow2 = (int)w && !( ((int)w-1) & (int)w );

		if( !isPow2 )
		{
			error("requested window size is not a power of 2. using default value of 2048 instead.");
			x->window = 2048;
		}
		else
			x->window = w;

		if(h<0)
		{
			error("hop value must be > 0. default value of 128 samples used instead.");
			x->hop = 128;
		}
		else
			x->hop = h;

		x->barkSpacing = 0.5;
	}
	else if(w)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
	    	pd_error(x, "%s: bad template for bark", x->x_arrayname->s_name);

		isPow2 = (int)w && !( ((int)w-1) & (int)w );

		if( !isPow2 )
		{
			error("requested window size is not a power of 2. using default value of 2048 instead.");
			x->window = 2048;
		}
		else
			x->window = w;

		x->hop = 128;

		x->barkSpacing = 0.5;
	}
	else if(s)
	{
		x->x_arrayname = s;

	    if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
	        ;
	    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
	    	pd_error(x, "%s: bad template for bark", x->x_arrayname->s_name);

		x->window = 2048;
		x->hop = 128;
		x->barkSpacing = 0.5;
	}
	else
	{
		error("bark: no array specified.");
		x->window = 2048;
		x->hop = 128;
		x->barkSpacing = 0.5;
	}

	x->debug = 0;
	x->sr = 44100.0;
	x->n = BLOCKSIZE;
	x->windowFunction = 4; // 4 is hann window
	x->powerSpectrum = 1; // choose mag (0) or power (1) spec
	x->normalize = 0;
	x->filterAvg = 0;
	x->loThresh = 3;
	x->hiThresh = 7;
	x->minvel = 1.0;
	x->haveHit = 0;
	x->debounceTime = 20;
	x->debounceSamp = x->debounceTime*0.001*x->sr;
	x->debounceActive = -1;
	x->maskDecay = 0.7;
	x->maskPeriods = 4;
	x->numFilters = 0;
	x->prevTotalGrowth = 0.0;
	x->useWeights = 0;
	x->spew = 0;

	x->signalBuf = (t_sample *)t_getbytes(x->window * sizeof(t_sample));
	x->analysisBuf = (t_float *)t_getbytes(x->window * sizeof(t_float));

	for(i=0; i<x->window; i++)
	{
		x->signalBuf[i] = 0.0;
		x->analysisBuf[i] = 0.0;
	}

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

	x->loBin = 0;
	x->hiBin = x->numFilters-1;

	x->mask = (t_float *)t_getbytes(x->numFilters*sizeof(t_float));
	x->growth = (t_float *)t_getbytes(x->numFilters*sizeof(t_float));
	x->numPeriods = (int *)t_getbytes(x->numFilters*sizeof(int));
	x->growthList = (t_atom *)t_getbytes(x->numFilters*sizeof(t_atom));
	x->loudWeights = (t_float *)t_getbytes(x->numFilters*sizeof(t_float));

	for(i=0; i<x->numFilters; i++)
	{
		x->mask[i] = 0.0;
		x->growth[i] = 0.0;
		x->numPeriods[i] = 0.0;
		SETFLOAT(x->growthList+i, 0.0);
		x->loudWeights[i] = 0.0;
	}

	bark_create_loudness_weighting(x);

    post("bark 0.0.6: window size: %i, hop: %i", (int)x->window, x->hop);

    return (x);
}


static void bark_analyze(t_bark *x, t_floatarg startTime, t_floatarg endTime)
{
    int i, j, window, windowHalf, hop, nFrames, frame, sampRange, startSamp, endSamp;
    t_float totalGrowth, totalVel, *windowFuncPtr;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for bark", x->x_arrayname->s_name);
	else
	{
		window = x->window;
		windowHalf = window*0.5;
		hop = x->hop;

		if(endTime)
		{
			startSamp = floor(startTime*x->sr);
			endSamp = floor(endTime*x->sr);

			if(startSamp>=0 && endSamp<x->x_array_points)
				sampRange = endSamp-startSamp+1;
			else
			{
				error("invalid time range");
				return;
			}
		}
		else
		{
			sampRange = x->x_array_points;
			startSamp = 0;
			endSamp = x->x_array_points-1;
		}

		nFrames = floor((sampRange-window)/hop);

		// init mask to zero
		for(i=0; i<x->numFilters; i++)
			x->mask[i] = 0.0;

		for(frame=0; frame<nFrames; frame++)
		{
			// fill buffer with <window> samples
			for(i=0, j=frame*hop+startSamp; i<window; i++, j++)
				x->signalBuf[i] = x->x_vec[j].w_float;

			totalGrowth = 0.0;
			totalVel = 0.0;

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
				    x->analysisBuf[i] = x->signalBuf[i] * *windowFuncPtr;
		    else
			    for(i=0; i<window; i++, windowFuncPtr++)
				    x->analysisBuf[i] = x->signalBuf[i];

			mayer_realfft(window, x->analysisBuf);

			// calculate the power spectrum in place. we'll overwrite the first N/2+1 points in x->analysisBuf with the magnitude spectrum, as this is all that's used below in filterbank_multiply()
			x->analysisBuf[0] = x->analysisBuf[0] * x->analysisBuf[0];  // DC
			x->analysisBuf[windowHalf] = x->analysisBuf[windowHalf] * x->analysisBuf[windowHalf];  // Nyquist

			for(i=(window-1), j=1; i>windowHalf; i--, j++)
				x->analysisBuf[j] = (x->analysisBuf[j]*x->analysisBuf[j]) + (x->analysisBuf[i]*x->analysisBuf[i]);

			// optional use of power/magnitude spectrum
			if(!x->powerSpectrum)
				for(i=0; i<=windowHalf; i++)
					x->analysisBuf[i] = sqrt(x->analysisBuf[i]);

			tIDLib_filterbankMultiply(x->analysisBuf, x->normalize, x->filterAvg, x->x_filterbank, x->numFilters);

			// optional loudness weighting
			if(x->useWeights)
				for(i=0; i<x->numFilters; i++)
					x->analysisBuf[i] *= x->loudWeights[i];

			for(i=0; i<x->numFilters; i++)
				totalVel += x->analysisBuf[i];

			// init growth list to zero
			for(i=0; i<x->numFilters; i++)
				x->growth[i] = 0.0;

			for(i=0; i<x->numFilters; i++)
			{
				// from p.3 of Puckette/Apel/Zicarelli, 1998
				// salt divisor with + 1.0e-15 in case previous power was zero
				if(x->analysisBuf[i] > x->mask[i])
					x->growth[i] = x->analysisBuf[i]/(x->mask[i] + 1.0e-15) - 1.0;

				if(i>=x->loBin && i<=x->hiBin && x->growth[i]>0)
					totalGrowth += x->growth[i];

				SETFLOAT(x->growthList+i, x->growth[i]);
			}

			if(frame*hop+startSamp >= x->debounceActive)
			    x->debounceActive = -1;

			if(totalVel >= x->minvel && totalGrowth > x->hiThresh && !x->haveHit && x->debounceActive < 0)
			{
				if(x->debug)
					post("peak: %f", totalGrowth);

				x->haveHit = 1;
				x->debounceActive = frame*hop+startSamp + x->debounceSamp;
			}
			else if(x->haveHit && x->loThresh>0 && totalGrowth < x->loThresh) // if loThresh is an actual value (not -1), then wait until growth drops below that value before reporting attack
			{
				if(x->debug)
					post("drop: %f", totalGrowth);

				x->haveHit = 0;

				// don't output data if spew will do it anyway below
				if(!x->spew)
				{
					outlet_list(x->x_outputList, 0, x->numFilters, x->growthList);
					outlet_float(x->x_growthOut, totalGrowth);
				}

				// add half a window of samples as a fudge factor. note that since this NRT and we can look into the future, all attack reports will be roughly a half window EARLY.  in RT, everything is a half window LATE because the point of reference is the END of the window.  here, it's the BEGINNING of the window.
				outlet_float(x->x_timeOut, (frame*hop+startSamp + windowHalf)/x->sr);
			}
			else if(x->haveHit && x->loThresh<0 && totalGrowth < x->prevTotalGrowth) // if loThresh == -1, report attack as soon as growth shows any decay at all
			{
				if(x->debug)
					post("drop: %f", totalGrowth);

				x->haveHit = 0;

				// don't output data if spew will do it anyway below
				if(!x->spew)
				{
					outlet_list(x->x_outputList, 0, x->numFilters, x->growthList);
					outlet_float(x->x_growthOut, totalGrowth);
				}

				// add half a window of samples as a fudge factor. note that since this NRT and we can look into the future, all attack reports will be roughly a half window EARLY.  in RT, everything is a half window LATE because the point of reference is the END of the window.  here, it's the BEGINNING of the window.
				outlet_float(x->x_timeOut, (frame*hop+startSamp + windowHalf)/x->sr);
			}

			if(x->spew)
			{
				outlet_list(x->x_outputList, 0, x->numFilters, x->growthList);
				outlet_float(x->x_growthOut, totalGrowth);
			}

			// update mask
			for(i=0; i<x->numFilters; i++)
			{
				if(x->analysisBuf[i] > x->mask[i])
				{
					x->mask[i] = x->analysisBuf[i];
					x->numPeriods[i] = 0;
				}
				else
					if(++x->numPeriods[i] >= x->maskPeriods)
						x->mask[i] *= x->maskDecay;
			}

			x->prevTotalGrowth = totalGrowth;
		}
	}

	post("analyzed %i frames", nFrames);
}


static void bark_free(t_bark *x)
{
	int i;

	// free the output list
	t_freebytes(x->growthList, x->numFilters * sizeof(t_atom));

	// free the input buffer memory
    t_freebytes(x->signalBuf, x->window*sizeof(t_sample));

	// free the analysis buffer memory
    t_freebytes(x->analysisBuf, x->window*sizeof(t_float));

	// free the window memory
    t_freebytes(x->blackman, x->window*sizeof(t_float));
    t_freebytes(x->cosine, x->window*sizeof(t_float));
    t_freebytes(x->hamming, x->window*sizeof(t_float));
    t_freebytes(x->hann, x->window*sizeof(t_float));

	// free the mask memory
    t_freebytes(x->mask, x->numFilters*sizeof(t_float));

	// free the growth record memory
    t_freebytes(x->growth, x->numFilters*sizeof(t_float));

	// free the mask counter memory
    t_freebytes(x->numPeriods, x->numFilters*sizeof(int));

	// free the loudness weights memory
    t_freebytes(x->loudWeights, x->numFilters*sizeof(t_float));

	// free the filterFreqs memory
	t_freebytes(x->x_filterFreqs, x->sizeFilterFreqs*sizeof(t_float));

    // free the filterbank memory
	for(i=0; i<x->numFilters; i++)
		t_freebytes(x->x_filterbank[i].filter, x->x_filterbank[i].size*sizeof(t_float));

	t_freebytes(x->x_filterbank, x->numFilters*sizeof(t_filter));
}


void bark_setup(void)
{
    bark_class =
    class_new(
    	gensym("bark"),
    	(t_newmethod)bark_new,
    	(t_method)bark_free,
        sizeof(t_bark),
        CLASS_DEFAULT,
        A_DEFSYM,
		A_DEFFLOAT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
    );

	class_addmethod(
		bark_class,
        (t_method)bark_analyze,
		gensym("analyze"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_windowFunction,
		gensym("window_function"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_thresh,
		gensym("thresh"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_minvel,
		gensym("minvel"),
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_filter_range,
		gensym("filter_range"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_mask,
		gensym("mask"),
        A_DEFFLOAT,
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_debounce,
		gensym("debounce"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_spew,
		gensym("spew"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_print,
		gensym("print"),
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_debug,
		gensym("debug"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_use_weights,
		gensym("loudness"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_samplerate,
		gensym("samplerate"),
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_filterFreqs,
		gensym("filter_freqs"),
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_hat,
		gensym("hat"),
        A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_filter_avg,
		gensym("filter_avg"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_powerSpectrum,
		gensym("power_spectrum"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		bark_class,
        (t_method)bark_normalize,
		gensym("normalize"),
		A_DEFFLOAT,
		0
	);
}

