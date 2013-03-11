/*

binWrangler - an external for organizing multiple frame feature vectors into component tracks.

Copyright 2010 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

version 0.0.4, December 21, 2011

¥ 0.0.4 adds permanent x_listOut memory and renames variables to get rid of underscores
¥ 0.0.3 adds a spew mode
¥ 0.0.2 as part of the timbreID-0.5 update, this gets rid of unnecessary getbytes(0) calls and uses memset() or memcpy() where possible

*/


#include "m_pd.h"

static t_class *binWrangler_class;

typedef struct instance
{
    float *instance;
} t_instance;

typedef struct _binWrangler
{
	t_object  x_obj;
    t_instance *instances;
    t_atom *x_listOut;
    int spew;
	int featureLength;
	int numFrames;
	int currentFrame;
    t_outlet *binList;
} t_binWrangler;


static void binWrangler_accum(t_binWrangler *x, t_symbol *s, int argc, t_atom *argv)
{
	int i, j, count, totalbins;
	s=s;

	if(x->featureLength != argc)
		error("binWrangler: input length does not match current length setting. input ignored.");
	else
		for(i=0; i<x->featureLength; i++)
			x->instances[x->currentFrame].instance[i] = atom_getfloat(argv+i);

	x->currentFrame++;

	if((x->currentFrame==x->numFrames) || (x->spew))
	{
		totalbins = x->numFrames*x->featureLength;

		for(i=0; i<x->featureLength; i++)
			for(count=0, j=x->numFrames-x->currentFrame; count<x->numFrames; count++, j++)
				SETFLOAT(x->x_listOut+(i*x->numFrames)+(j%x->numFrames), x->instances[count].instance[i]);

		outlet_list(x->binList, 0, totalbins, x->x_listOut);

		x->currentFrame = (x->currentFrame==x->numFrames)?0:x->currentFrame;
	}
}

static void binWrangler_clear(t_binWrangler *x)
{
	int i, j;

	// free the database memory
	for(i=0; i<x->numFrames; i++)
		t_freebytes(x->instances[i].instance, x->featureLength*sizeof(float));

	t_freebytes(x->instances, x->numFrames*sizeof(t_instance));

	x->currentFrame = 0;

    for(i=0; i<x->featureLength*x->numFrames; i++)
        SETFLOAT(x->x_listOut+i, 0.0);

    x->instances = (t_instance *)t_getbytes(x->numFrames*sizeof(t_instance));

	for(i=0; i<x->numFrames; i++)
		x->instances[i].instance = (float *)t_getbytes(x->featureLength*sizeof(float));

	for(i=0; i<x->numFrames; i++)
		for(j=0; j<x->featureLength; j++)
			x->instances[i].instance[j] = 0.0;
}

static void binWrangler_numFrames(t_binWrangler *x, t_float num)
{
	int i, j;

	if(num)
	{
        x->x_listOut = (t_atom *)t_resizebytes(x->x_listOut, (x->featureLength*x->numFrames)*sizeof(t_atom), (x->featureLength*num)*sizeof(t_atom));

		// free the database memory
		for(i=0; i<x->numFrames; i++)
			t_freebytes(x->instances[i].instance, x->featureLength*sizeof(float));

		t_freebytes(x->instances, x->numFrames*sizeof(t_instance));

		x->currentFrame = 0;
		x->numFrames = num;

        for(i=0; i<x->featureLength*x->numFrames; i++)
	        SETFLOAT(x->x_listOut+i, 0.0);

		x->instances = (t_instance *)t_getbytes(x->numFrames*sizeof(t_instance));

		for(i=0; i<x->numFrames; i++)
			x->instances[i].instance = (float *)t_getbytes(x->featureLength*sizeof(float));

		for(i=0; i<x->numFrames; i++)
			for(j=0; j<x->featureLength; j++)
				x->instances[i].instance[j] = 0.0;

	}
}

static void binWrangler_length(t_binWrangler *x, t_float len)
{
	int i, j;

	if(len)
	{
        x->x_listOut = (t_atom *)t_resizebytes(x->x_listOut, (x->featureLength*x->numFrames)*sizeof(t_atom), (len*x->numFrames)*sizeof(t_atom));

		// free the database memory
		for(i=0; i<x->numFrames; i++)
			t_freebytes(x->instances[i].instance, x->featureLength*sizeof(float));

		t_freebytes(x->instances, x->numFrames*sizeof(t_instance));

		x->instances = (t_instance *)t_getbytes(x->numFrames*sizeof(t_instance));

		x->featureLength = len;
		x->currentFrame = 0;

        for(i=0; i<x->featureLength*x->numFrames; i++)
	        SETFLOAT(x->x_listOut+i, 0.0);

		for(i=0; i<x->numFrames; i++)
			x->instances[i].instance = (float *)t_getbytes(x->featureLength*sizeof(float));

		for(i=0; i<x->numFrames; i++)
			for(j=0; j<x->featureLength; j++)
				x->instances[i].instance[j] = 0.0;
	}
}

static void binWrangler_spew(t_binWrangler *x, t_floatarg s)
{
	s = (s<=0)?0:s;
	s = (s>=1)?1:s;
	x->spew = s;

	post("spew mode: %i", x->spew);
}

static void *binWrangler_new(t_float numFrames, t_float length, t_float spew)
{
	t_binWrangler *x = (t_binWrangler *)pd_new(binWrangler_class);
	int i, j;

	x->binList = outlet_new(&x->x_obj, gensym("list"));

	x->featureLength = length;
	x->numFrames = numFrames;
	x->currentFrame = 0;
	spew = (spew>1)?1:spew;
	spew = (spew<0)?0:spew;
	x->spew = spew;


    x->x_listOut = (t_atom *)t_getbytes((x->featureLength*x->numFrames)*sizeof(t_atom));
    x->instances = (t_instance *)t_getbytes(x->numFrames*sizeof(t_instance));

	for(i=0; i<x->featureLength*x->numFrames; i++)
		SETFLOAT(x->x_listOut+i, 0.0);

	for(i=0; i<x->numFrames; i++)
		x->instances[i].instance = (float *)t_getbytes(x->featureLength*sizeof(float));

	for(i=0; i<x->numFrames; i++)
		for(j=0; j<x->featureLength; j++)
			x->instances[i].instance[j] = 0.0;

	return (void *)x;
}

static void binWrangler_free(t_binWrangler *x)
{
	int i;

	// free listOut memory
	t_freebytes(x->x_listOut, (x->featureLength*x->numFrames)*sizeof(t_atom));

	// free the database memory
	for(i=0; i<x->currentFrame; i++)
		t_freebytes(x->instances[i].instance, x->featureLength*sizeof(float));

	t_freebytes(x->instances, x->numFrames*sizeof(t_instance));

}

void binWrangler_setup(void) {

	binWrangler_class = class_new(gensym("binWrangler"),
		(t_newmethod)binWrangler_new,
		(t_method)binWrangler_free,
		sizeof(t_binWrangler),
		CLASS_DEFAULT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addlist(binWrangler_class, binWrangler_accum);

	class_addmethod(
		binWrangler_class,
        (t_method)binWrangler_clear,
		gensym("clear"),
		0
	);

	class_addmethod(
		binWrangler_class,
        (t_method)binWrangler_numFrames,
		gensym("num_frames"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		binWrangler_class,
        (t_method)binWrangler_length,
		gensym("length"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		binWrangler_class,
        (t_method)binWrangler_spew,
		gensym("spew"),
		A_DEFFLOAT,
		0
	);
}

