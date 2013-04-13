/*

tabletool - An array manipulation external.

Copyright 2010 William Brent

tabletool is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

tabletool is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

version 0.0.8, December 21, 2011

• 0.0.8 adds a dump_range method
• 0.0.7 just gets rid of some unused parameter warnings from the hann/blackman window functions
• 0.0.6 adds Hann/Blackman windows, a random fill command, a peak picker, change function. Fixed bug in shift function, and  updated shift0 to be a consistent format with the new shift.
• 0.0.5 adds an add_range method for mixing down parts of tables
• 0.0.4 as part of the timbreID-0.5 update, this gets rid of unnecessary getbytes(0) calls and uses memset() or memcpy() where possible. fixed a mistake in the _normalize function
• 0.0.3 improved scramble function. No longer trying to load arrays in _new
• 0.0.2 added a const function, and a series function.

*/


#include "m_pd.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

static t_class *tabletool_class;

typedef struct _tabletool
{
    t_object x_obj;
	t_word *x_vec;
    unsigned int rand_state;
	t_symbol *x_arrayname;
	int stored_flag;
	t_float *original_data;
	int x_array_points;
    t_outlet *x_info;
    t_outlet *x_list;

} t_tabletool;



static void tabletool_bubble_sort(int n, t_float *list)
{
	int i, j, flag;

	for(i=0; i<n; i++)
	{
		flag = 0;

		for(j=0; j<(n-1); j++)
		{
			if(list[j] > list[j+1])
			{
				float tmp;

				flag = 1;

				tmp = list[j+1];
				list[j+1] = list[j];
				list[j] = tmp;
			}
		}

		if(flag==0)
			break;
	}
}

/* ------------------------ tabletool -------------------------------- */

static void tabletool_dump(t_tabletool *x)
{
	int i;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		list_out = (t_atom *)t_getbytes(x->x_array_points*sizeof(t_atom));

		for(i=0; i<x->x_array_points; i++)
			SETFLOAT(list_out+i, x->x_vec[i].w_float);

		outlet_list(x->x_list, 0, x->x_array_points, list_out);

		// free local memory
		t_freebytes(list_out, x->x_array_points * sizeof(t_atom));
 	}
}


static void tabletool_dumpRange(t_tabletool *x, t_floatarg start, t_floatarg finish)
{
	int i, j;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		int tmp, startIdx, finishIdx, range;
		
		start = (start<0)?0:start;
		start = (start>=x->x_array_points)?x->x_array_points-1:start;
		startIdx = start;

		finish = (finish<0)?0:finish;
		finish = (finish>=x->x_array_points)?x->x_array_points-1:finish;
		finishIdx = finish;
		
		if(startIdx>finishIdx)
		{
			tmp = finishIdx;
			finishIdx = startIdx;
			startIdx = tmp;
		}
		
		range = finishIdx-startIdx+1;
		
		list_out = (t_atom *)t_getbytes(range*sizeof(t_atom));

		for(i=0, j=startIdx; j<=finishIdx; i++, j++)
			SETFLOAT(list_out+i, x->x_vec[j].w_float);

		outlet_list(x->x_list, 0, range, list_out);

		// free local memory
		t_freebytes(list_out, range*sizeof(t_atom));
 	}
}


static void tabletool_size(t_tabletool *x)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
		outlet_float(x->x_info, x->x_array_points);
}


static void tabletool_min(t_tabletool *x)
{
	t_garray *a;
	int i;
	t_float min;
	t_atom *index_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		index_out = (t_atom *)t_getbytes(sizeof(t_atom));

		min = FLT_MAX;

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float < min)
			{
				min = x->x_vec[i].w_float;
				SETFLOAT(index_out, i);
			}

		outlet_list(x->x_list, 0, 1, index_out);
		outlet_float(x->x_info, min);

		// free local memory
		t_freebytes(index_out, sizeof(t_atom));
	}
}


static void tabletool_max(t_tabletool *x)
{
	t_garray *a;
	int i;
	t_float max;
	t_atom *index_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		index_out = (t_atom *)t_getbytes(sizeof(t_atom));

		max = -1 * FLT_MAX;

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float > max)
			{
				max = x->x_vec[i].w_float;
				SETFLOAT(index_out, i);
			}

		outlet_list(x->x_list, 0, 1, index_out);
		outlet_float(x->x_info, max);

		// free local memory
		t_freebytes(index_out, sizeof(t_atom));
	}
}


static void tabletool_nearest(t_tabletool *x, t_float val)
{
	t_garray *a;
	int i, idx;
	t_float dist;
	t_atom *index_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		index_out = (t_atom *)t_getbytes(sizeof(t_atom));

		dist = FLT_MAX;
		idx = 0;

		for(i=0; i<x->x_array_points; i++)
			if(fabs(x->x_vec[i].w_float - val) < dist)
			{
				dist = fabs(x->x_vec[i].w_float - val);
				SETFLOAT(index_out, i);
				idx = i;
			}

		outlet_list(x->x_list, 0, 1, index_out);
		outlet_float(x->x_info, x->x_vec[idx].w_float);

		// free local memory
		t_freebytes(index_out, sizeof(t_atom));
	}
}


static void tabletool_equals(t_tabletool *x, t_float val)
{
	int i, num_matches, *indices;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		num_matches = 0;

		indices = (int *)t_getbytes(0);

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float == val)
			{
				num_matches++;

				indices = (int *)t_resizebytes(indices, (num_matches-1)*sizeof(int), num_matches*sizeof(int));

				indices[num_matches-1] = i;
			}

		if(num_matches)
		{
			list_out = (t_atom *)t_getbytes(num_matches*sizeof(t_atom));

			for(i=0; i<num_matches; i++)
				SETFLOAT(list_out+i, indices[i]);

			outlet_list(x->x_list, 0, num_matches, list_out);
			outlet_float(x->x_info, num_matches);

			// free local memory
			t_freebytes(list_out, num_matches * sizeof(t_atom));
		}
		else
			outlet_float(x->x_info, -1);

		// free local memory
		t_freebytes(indices, num_matches * sizeof(int));
 	}
}


static void tabletool_greater(t_tabletool *x, t_float val)
{
	int i, num_matches, *indices;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		num_matches = 0;

		indices = (int *)t_getbytes(0);

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float > val)
			{
				num_matches++;

				indices = (int *)t_resizebytes(indices, (num_matches-1)*sizeof(int), num_matches*sizeof(int));

				indices[num_matches-1] = i;
			}

		if(num_matches)
		{
			list_out = (t_atom *)t_getbytes(num_matches*sizeof(t_atom));

			for(i=0; i<num_matches; i++)
				SETFLOAT(list_out+i, indices[i]);

			outlet_list(x->x_list, 0, num_matches, list_out);
			outlet_float(x->x_info, num_matches);

			// free local memory
			t_freebytes(list_out, num_matches * sizeof(t_atom));
		}
		else
			outlet_float(x->x_info, -1);

		// free local memory
		t_freebytes(indices, num_matches * sizeof(int));
 	}
}


static void tabletool_less(t_tabletool *x, t_float val)
{
	int i, num_matches, *indices;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		num_matches = 0;

		indices = (int *)t_getbytes(0);

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float < val)
			{
				num_matches++;

				indices = (int *)t_resizebytes(indices, (num_matches-1)*sizeof(int), num_matches*sizeof(int));

				indices[num_matches-1] = i;
			}

		if(num_matches)
		{
			list_out = (t_atom *)t_getbytes(num_matches*sizeof(t_atom));

			for(i=0; i<num_matches; i++)
				SETFLOAT(list_out+i, indices[i]);

			outlet_list(x->x_list, 0, num_matches, list_out);
			outlet_float(x->x_info, num_matches);

			// free local memory
			t_freebytes(list_out, num_matches * sizeof(t_atom));
		}
		else
			outlet_float(x->x_info, -1);

		// free local memory
		t_freebytes(indices, num_matches * sizeof(int));
 	}
}


static void tabletool_between(t_tabletool *x, t_float low_bound, t_float hi_bound)
{
	int i, num_matches, *indices;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		num_matches = 0;

		indices = (int *)t_getbytes(0);

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float > low_bound && x->x_vec[i].w_float < hi_bound)
			{
				num_matches++;

				indices = (int *)t_resizebytes(indices, (num_matches-1)*sizeof(int), num_matches*sizeof(int));

				indices[num_matches-1] = i;
			}

		if(num_matches)
		{
			list_out = (t_atom *)t_getbytes(num_matches*sizeof(t_atom));

			for(i=0; i<num_matches; i++)
				SETFLOAT(list_out+i, indices[i]);

			outlet_list(x->x_list, 0, num_matches, list_out);
			outlet_float(x->x_info, num_matches);

			// free local memory
			t_freebytes(list_out, num_matches * sizeof(t_atom));
		}
		else
			outlet_float(x->x_info, -1);

		// free local memory
		t_freebytes(indices, num_matches * sizeof(int));
 	}
}


static void tabletool_choose(t_tabletool *x)
{
	int nval;
	unsigned int randval;
	t_atom *index_out;
	float choice;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		index_out = (t_atom *)t_getbytes(sizeof(t_atom));

		randval = x->rand_state;
		x->rand_state = randval = randval * 472940017 + 832416023; // from random in x_misc.c
		nval = ((double)x->x_array_points) * ((double)randval)
			* (1./4294967296.);

		nval = (int)nval % x->x_array_points;
		SETFLOAT(index_out, nval);

		choice = x->x_vec[nval].w_float;

		outlet_list(x->x_list, 0, 1, index_out);
		outlet_float(x->x_info, choice);

		// free local memory
		t_freebytes(index_out, sizeof(t_atom));
	}
}


static void tabletool_const(t_tabletool *x, t_float val, t_float idx1, t_float idx2)
{
	t_garray *a;
	int i, iidx1, iidx2;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		iidx1 = idx1;
		iidx2 = idx2;

		for(i=iidx1; i<=iidx2; i++)
			x->x_vec[i].w_float = val;

		garray_redraw(a);
	}
}


static void tabletool_series(t_tabletool *x, t_float startval, t_float endval, t_float exponent)
{
	t_garray *a;
	int i;
	float start, end, span, inc;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		exponent = (exponent<0)? 0 : exponent;

		start = startval;
		end = endval;
		span = end-start;
		inc = 1.0/(x->x_array_points-1);

		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float = pow(inc*i, exponent) * span + start;

		garray_redraw(a);
	}
}


static void tabletool_random_walk(t_tabletool *x, t_float start, t_float step, t_float lowlim, t_float uplim)
{
	t_garray *a;
	int i;
	double split;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		split = RAND_MAX * 0.5;

		x->x_vec[0].w_float = start;

		for(i=1; i<x->x_array_points; i++)
		{
			if(rand()>split)
				x->x_vec[i].w_float = x->x_vec[i-1].w_float + step;
			else
				x->x_vec[i].w_float = x->x_vec[i-1].w_float - step;

			if(x->x_vec[i].w_float > uplim)
				x->x_vec[i].w_float = uplim - (x->x_vec[i].w_float-uplim);
			else if(x->x_vec[i].w_float < lowlim)
				x->x_vec[i].w_float = lowlim + (lowlim-x->x_vec[i].w_float);
		}

		garray_redraw(a);
	}
}


static void tabletool_offset(t_tabletool *x, t_float offset)
{
	int i;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float += offset;

		garray_redraw(a);
 	}
}


static void tabletool_shift(t_tabletool *x, t_float s)
{
	int i, j, shift, lentable;
	t_garray *a;
	t_float *tablevals;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		shift = s; // cast to int
		lentable = x->x_array_points;
		shift = shift % lentable;

		for(i=0; i<lentable; i++)
			tablevals[i] = x->x_vec[i].w_float;

		if(shift>=0)
			for(i=shift, j=0; j<lentable; i++, j++)
				x->x_vec[i%lentable].w_float = tablevals[j];
		else
			for(i=0, j=abs(shift); i<lentable; i++, j++)
				x->x_vec[i].w_float = tablevals[j%lentable];

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, lentable * sizeof(t_float));
 	}
}


static void tabletool_shift0(t_tabletool *x, t_float s)
{
	int i, j, shift, lentable;
	t_garray *a;
	t_float *tablevals;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		shift = s; // cast to int
		lentable = x->x_array_points;
		shift = shift % lentable;

		for(i=0; i<lentable; i++)
			tablevals[i] = x->x_vec[i].w_float;

		if(shift>=0)
		{
			for(i=0; i<shift; i++)
				x->x_vec[i].w_float = 0.0;

			for(i=shift, j=0; i<lentable; i++, j++)
				x->x_vec[i].w_float = tablevals[j];
		}
		else
		{
			for(i=0, j=abs(shift); j<lentable; i++, j++)
				x->x_vec[i].w_float = tablevals[j];

			for(; i<lentable; i++)
				x->x_vec[i].w_float = 0.0;
		}

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, lentable * sizeof(t_float));
 	}
}


static void tabletool_scale(t_tabletool *x, t_float scalar)
{
	t_garray *a;
	int i;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		// rescale
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float *= scalar;

		garray_redraw(a);
	}
}


static void tabletool_invert(t_tabletool *x)
{
	t_garray *a;
	int i;
	t_float max, min;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		max = 0.0;
		min = FLT_MAX;

		for(i=0; i<x->x_array_points; i++)
		{
			if(x->x_vec[i].w_float < min)
				min = x->x_vec[i].w_float;

			if(x->x_vec[i].w_float > max)
				max = x->x_vec[i].w_float;
		}

		// rescale
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float = max - x->x_vec[i].w_float + min;

		garray_redraw(a);
	}
}


static void tabletool_reverse(t_tabletool *x)
{
	int i, j;
	t_float *tablevals;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		// load table data
		for(i=0; i<x->x_array_points; i++)
			tablevals[i] = x->x_vec[i].w_float;

		for(i=x->x_array_points-1, j=0; j<x->x_array_points; i--, j++)
			x->x_vec[j].w_float = tablevals[i];

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, x->x_array_points * sizeof(t_float));
    }
}


static void tabletool_remove(t_tabletool *x, t_float idx)
{
	t_garray *a;
	int i, iidx, lastidx;
	float val;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		lastidx = x->x_array_points-1;

		if(idx>lastidx)
			return;
		else if(idx<0)
			return;
		else
			iidx = idx;

		val = x->x_vec[iidx].w_float;

		outlet_float(x->x_info, val);

		for(i=iidx; i<lastidx; i++)
			x->x_vec[i].w_float = x->x_vec[i+1].w_float;

		x->x_vec[lastidx].w_float = 0.0;

		garray_redraw(a);
	}
}


static void tabletool_insert(t_tabletool *x, t_float idx, t_float val)
{
	t_garray *a;
	int i, iidx, lastidx;
	t_float *tablevals;


	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		// load table data
		for(i=0; i<x->x_array_points; i++)
			tablevals[i] = x->x_vec[i].w_float;

		lastidx = x->x_array_points-1;

		if(idx>lastidx)
			return;
		else if(idx<0)
			return;
		else
			iidx = idx;

		outlet_float(x->x_info, x->x_vec[lastidx].w_float);

		for(i=iidx; i<lastidx; i++)
			x->x_vec[i+1].w_float = tablevals[i];

		x->x_vec[iidx].w_float = val;

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, x->x_array_points * sizeof(t_float));
	}
}


static void tabletool_smooth(t_tabletool *x)
{
	int i;
	t_float *tablevals, threerecip;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
		pd_error(x, "%s: no such array", x->x_arrayname->s_name);
	else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
		pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{

		if(x->x_array_points < 3)
			post("tabletool: table smoothing cannot be performed on tables with fewer than 3 elements");
		else
		{
			tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

			threerecip = 1.0/3.0;

			// load table data
			for(i=0; i<x->x_array_points; i++)
				tablevals[i] = x->x_vec[i].w_float;

			x->x_vec[0].w_float = (tablevals[0] + tablevals[1] + tablevals[2])*threerecip;

			for(i=1; i<x->x_array_points-1; i++)
				x->x_vec[i].w_float = (tablevals[i-1] + tablevals[i] + tablevals[i+1])*threerecip;

			x->x_vec[x->x_array_points-1].w_float = (tablevals[x->x_array_points-1] + tablevals[x->x_array_points-2] + tablevals[x->x_array_points-3])*threerecip;

			garray_redraw(a);

			// free local memory
			t_freebytes(tablevals, x->x_array_points * sizeof(t_float));
		}
	}
}

static void tabletool_lace(t_tabletool *x, t_symbol *array2)
{
	int i, j, array2pts, totalpoints;
	t_garray *a, *b;
	t_atom *list_out;
	t_word *vec2;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array2, garray_class)))
		{
			pd_error(x, "%s: no such array", array2->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array2pts, &vec2))
		{
			pd_error(x, "%s: bad template for tabletool", array2->s_name);
			return;
		}

		totalpoints = x->x_array_points + array2pts;
		list_out = (t_atom *)t_getbytes(totalpoints*sizeof(t_atom));

		for(i=0, j=0; i<(x->x_array_points*2)-1; i+=2, j++)
		{
			if(j > array2pts-1)
				break;

			SETFLOAT(list_out+i, x->x_vec[j].w_float);
			SETFLOAT(list_out+i+1, vec2[j].w_float);
		}

		for(; j<x->x_array_points; i++, j++)
			SETFLOAT(list_out+i, x->x_vec[j].w_float);

		outlet_list(x->x_list, 0, totalpoints, list_out);

		// free local memory
		t_freebytes(list_out, totalpoints*sizeof(t_atom));
	}
}


static void tabletool_concatenate(t_tabletool *x, t_symbol *array2)
{
	int i, j, array2pts, totalpoints;
	t_garray *a, *b;
	t_word *vec2;
	t_atom *list_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array2, garray_class)))
		{
			pd_error(x, "%s: no such array", array2->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array2pts, &vec2))
		{
			pd_error(x, "%s: bad template for tabletool", array2->s_name);
			return;
		}

		totalpoints = x->x_array_points + array2pts;
		list_out = (t_atom *)t_getbytes(totalpoints*sizeof(t_atom));

		for(i=0, j=0; i<x->x_array_points; i++, j++)
			SETFLOAT(list_out+i, x->x_vec[j].w_float);


		for(j=0; i<totalpoints; i++, j++)
			SETFLOAT(list_out+i, vec2[j].w_float);

		outlet_list(x->x_list, 0, totalpoints, list_out);

		// free local memory
		t_freebytes(list_out, totalpoints*sizeof(t_atom));
	}
}


static void tabletool_scramble(t_tabletool *x)
{
    int i, length, range, nval, *indices, *rand_indices;
    t_float temp, *tablevals;
    unsigned int randval;
    t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{

		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		indices = (int *)t_getbytes(x->x_array_points*sizeof(int));

		rand_indices = (int *)t_getbytes(x->x_array_points*sizeof(int));

		for(i=0; i<x->x_array_points; i++)
			indices[i] = i;

		for(i=0; i<x->x_array_points; i++)
			tablevals[i] = x->x_vec[i].w_float;

		length = x->x_array_points;

		for(i=0; i<length-1; i++)
		{
			range = length-i;
			range = (range<1)?1:range;

			randval = x->rand_state;
			x->rand_state = randval = randval * 472940017 + 832416023; // from random in x_misc.c
			nval = ((double)range) * ((double)randval)
				* (1./4294967296.);

			nval = nval % range;
			nval += i;

			temp = tablevals[nval];
			tablevals[nval] = tablevals[i];
			tablevals[i] = temp;
		}

		for(i=0; i<x->x_array_points; i++)
			 x->x_vec[i].w_float = tablevals[i];

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, x->x_array_points * sizeof(t_float));
		t_freebytes(indices, x->x_array_points * sizeof(int));
		t_freebytes(rand_indices, x->x_array_points * sizeof(int));
	}
}


static void tabletool_sort(t_tabletool *x)
{
	int i;
	t_garray *a;
	t_float *tablevals;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		for(i=0; i<x->x_array_points; i++)
			tablevals[i] = x->x_vec[i].w_float;

		tabletool_bubble_sort(x->x_array_points, tablevals);

		for(i=0; i<x->x_array_points; i++)
			 x->x_vec[i].w_float = tablevals[i];

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, x->x_array_points * sizeof(t_float));
	}
}


static void tabletool_sort_range(t_tabletool *x, t_float start, t_float end)
{
	int i, j, si, ei, length;
	t_garray *a;
	t_float *tablevals;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		si = start;
		ei = end;

		if(si<0)
			si = 0;

		if(ei>x->x_array_points-1)
			ei = x->x_array_points-1;

		length = ei-si+1;

		tablevals = (t_float *)t_getbytes(length*sizeof(t_float));

		for(i=si, j=0; i<=ei; i++, j++)
			tablevals[j] = x->x_vec[i].w_float;

		tabletool_bubble_sort(length, tablevals);

		for(i=si, j=0; i<=ei; i++, j++)
			 x->x_vec[i].w_float = tablevals[j];

		garray_redraw(a);

		// free local memory
		t_freebytes(tablevals, length * sizeof(t_float));
	}
}


static void tabletool_swap(t_tabletool *x, t_float idx1, t_float idx2)
{
	int i1, i2;
	t_garray *a;
	t_float temp;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		i1 = idx1;
		i2 = idx2;

		if(i1<0 || i1>x->x_array_points-1 || i2<0 || i2>x->x_array_points-1)
		{
			error("tabletool: index out of bounds");
			return;
		}

		temp = x->x_vec[i1].w_float;

		x->x_vec[i1].w_float = x->x_vec[i2].w_float;
		x->x_vec[i2].w_float = temp;

		garray_redraw(a);
	}
}


static void tabletool_integrate(t_tabletool *x)
{
	int i;
	t_float sum;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		list_out = (t_atom *)t_getbytes(x->x_array_points*sizeof(t_atom));

		SETFLOAT(list_out, x->x_vec[0].w_float);

		sum = x->x_vec[0].w_float;

		for(i=1; i<x->x_array_points; i++)
		{
			SETFLOAT(list_out+i, x->x_vec[i].w_float + sum);
			sum += x->x_vec[i].w_float;
		}

		outlet_list(x->x_list, 0, x->x_array_points, list_out);

		// free local memory
		t_freebytes(list_out, x->x_array_points * sizeof(t_atom));
 	}
}


static void tabletool_differentiate(t_tabletool *x)
{
	int i;
	t_atom *list_out;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		list_out = (t_atom *)t_getbytes(x->x_array_points*sizeof(t_atom));

		SETFLOAT(list_out, x->x_vec[0].w_float);

		for(i=1; i<x->x_array_points; i++)
			SETFLOAT(list_out+i, x->x_vec[i].w_float - x->x_vec[i-1].w_float);

		outlet_list(x->x_list, 0, x->x_array_points, list_out);

		// free local memory
		t_freebytes(list_out, x->x_array_points * sizeof(t_atom));
 	}
}


static void tabletool_add(t_tabletool *x, t_symbol *array2)
{
	int i, array2pts, totalpoints;
	t_garray *a, *b;
	t_word *vec2;
	t_atom *list_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array2, garray_class)))
		{
			pd_error(x, "%s: no such array", array2->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array2pts, &vec2))
		{
			pd_error(x, "%s: bad template for tabletool", array2->s_name);
			return;
		}

		totalpoints = x->x_array_points + array2pts;

		list_out = (t_atom *)t_getbytes(totalpoints*sizeof(t_atom));

		for(i=0; i<x->x_array_points; i++)
			if(i>array2pts-1)
				break;
			else
				SETFLOAT(list_out+i, x->x_vec[i].w_float + vec2[i].w_float);

		outlet_list(x->x_list, 0, totalpoints, list_out);

		// free local memory
		t_freebytes(list_out, totalpoints * sizeof(t_atom));
	}
}


static void tabletool_add_range(t_tabletool *x, t_symbol *s, int argc, t_atom *argv)
{
	int i, j, sourcepts, ts, ss, se, length;
	t_garray *a, *b;
	t_word *sourcevec;
	t_symbol *source;
	t_atom *list_out;

	argc=argc;
	s=s;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		ts = atom_getfloat(argv+0);
		source = atom_getsymbol(argv+1);
		ss = atom_getfloat(argv+2);
		se = atom_getfloat(argv+3);

		if(!(b = (t_garray *)pd_findbyclass(source, garray_class)))
		{
			pd_error(x, "%s: no such array", source->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &sourcepts, &sourcevec))
		{
			pd_error(x, "%s: bad template for tabletool", source->s_name);
			return;
		}

		if(ts<0)
			ts = 0;

		if(ss<0)
			ss = 0;

		if(se>sourcepts-1)
			se = sourcepts-1;

		length = se-ss+1;

		list_out = (t_atom *)t_getbytes(length*sizeof(t_atom));

		for(i=0, j=ss; j<=se; i++, j++)
			if(i > x->x_array_points-1)
				break;
			else
				SETFLOAT(list_out+i, sourcevec[j].w_float + x->x_vec[ts+i].w_float);

		outlet_list(x->x_list, 0, length, list_out);

		// free local memory
		t_freebytes(list_out, length * sizeof(t_atom));
	}
}



static void tabletool_subtract(t_tabletool *x, t_symbol *array2)
{
	int i, array2pts, totalpoints;
	t_garray *a, *b;
	t_word *vec2;
	t_atom *list_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array2, garray_class)))
		{
			pd_error(x, "%s: no such array", array2->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array2pts, &vec2))
		{
			pd_error(x, "%s: bad template for tabletool", array2->s_name);
			return;
		}

		totalpoints = x->x_array_points + array2pts;

		list_out = (t_atom *)t_getbytes(totalpoints*sizeof(t_atom));

		for(i=0; i<x->x_array_points; i++)
			if(i>array2pts-1)
				break;
			else
				SETFLOAT(list_out+i, x->x_vec[i].w_float - vec2[i].w_float);

		outlet_list(x->x_list, 0, totalpoints, list_out);

		// free local memory
		t_freebytes(list_out, totalpoints * sizeof(t_atom));
	}
}


static void tabletool_multiply(t_tabletool *x, t_symbol *array2)
{
	int i, array2pts, totalpoints;
	t_garray *a, *b;
	t_word *vec2;
	t_atom *list_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array2, garray_class)))
		{
			pd_error(x, "%s: no such array", array2->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array2pts, &vec2))
		{
			pd_error(x, "%s: bad template for tabletool", array2->s_name);
			return;
		}

		totalpoints = x->x_array_points + array2pts;

		list_out = (t_atom *)t_getbytes(totalpoints*sizeof(t_atom));

		for(i=0; i<x->x_array_points; i++)
			if(i>array2pts-1)
				break;
			else
				SETFLOAT(list_out+i, x->x_vec[i].w_float * vec2[i].w_float);

		outlet_list(x->x_list, 0, totalpoints, list_out);

		// free local memory
		t_freebytes(list_out, totalpoints * sizeof(t_atom));
	}
}


static void tabletool_divide(t_tabletool *x, t_symbol *array2)
{
	int i, array2pts, totalpoints;
	t_garray *a, *b;
	t_word *vec2;
	t_atom *list_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array2, garray_class)))
		{
			pd_error(x, "%s: no such array", array2->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array2pts, &vec2))
		{
			pd_error(x, "%s: bad template for tabletool", array2->s_name);
			return;
		}

		totalpoints = x->x_array_points + array2pts;

		list_out = (t_atom *)t_getbytes(totalpoints*sizeof(t_atom));

		for(i=0; i<x->x_array_points; i++)
			if(i>array2pts-1)
				break;
			else
				if(vec2[i].w_float == 0)
				{
					error("tabletool: divide by zero detected");
					SETFLOAT(list_out+i, x->x_vec[i].w_float);
				}
				else
					SETFLOAT(list_out+i, x->x_vec[i].w_float / vec2[i].w_float);


		outlet_list(x->x_list, 0, totalpoints, list_out);

		// free local memory
		t_freebytes(list_out, totalpoints * sizeof(t_atom));
	}
}


static void tabletool_dot(t_tabletool *x, t_symbol *array1)
{
	int i, array1pts;
	t_garray *a, *b;
	t_word *vec1;
	t_float dot;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array1, garray_class)))
		{
			pd_error(x, "%s: no such array", array1->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array1pts, &vec1))
		{
			pd_error(x, "%s: bad template for tabletool", array1->s_name);
			return;
		}

		dot = 0;

		for(i=0; i<array1pts; i++)
			if(i>x->x_array_points-1)
				break;
			else
				dot += x->x_vec[i].w_float * vec1[i].w_float;

		outlet_float(x->x_info, dot);
	}
}


static void tabletool_euclid(t_tabletool *x, t_symbol *array1)
{
	int i, array1pts;
	t_garray *a, *b;
	t_word *vec1;
	t_float diff, euclid;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array1, garray_class)))
		{
			pd_error(x, "%s: no such array", array1->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array1pts, &vec1))
		{
			pd_error(x, "%s: bad template for tabletool", array1->s_name);
			return;
		}

		euclid = diff = 0;

		for(i=0; i<array1pts; i++)
			if(i>x->x_array_points-1)
				break;
			else
			{
				diff = x->x_vec[i].w_float - vec1[i].w_float;
				euclid += diff*diff;
			}

		outlet_float(x->x_info, euclid);
	}
}


static void tabletool_taxi(t_tabletool *x, t_symbol *array1)
{
	int i, array1pts;
	t_garray *a, *b;
	t_word *vec1;
	t_float diff, euclid;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array1, garray_class)))
		{
			pd_error(x, "%s: no such array", array1->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array1pts, &vec1))
		{
			pd_error(x, "%s: bad template for tabletool", array1->s_name);
			return;
		}

		euclid = diff = 0;

		for(i=0; i<array1pts; i++)
			if(i>x->x_array_points-1)
				break;
			else
			{
				diff = fabs(x->x_vec[i].w_float - vec1[i].w_float);
				euclid += diff;
			}

		outlet_float(x->x_info, euclid);
	}
}


static void tabletool_corr(t_tabletool *x, t_symbol *array1)
{
	int i, array1pts;
	t_garray *a, *b;
	t_word *vec1;
	t_float sum, mean, std, sum1, mean1, std1, corr, *vec_centered, *vec1_centered;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(!(b = (t_garray *)pd_findbyclass(array1, garray_class)))
		{
			pd_error(x, "%s: no such array", array1->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &array1pts, &vec1))
		{
			pd_error(x, "%s: bad template for tabletool", array1->s_name);
			return;
		}

		vec_centered = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		vec1_centered = (t_float *)t_getbytes(array1pts*sizeof(t_float));

		sum = sum1 = mean = mean1 = std = std1 = corr = 0.0;

		for(i=0; i<x->x_array_points; i++)
			sum += x->x_vec[i].w_float;

		for(i=0; i<array1pts; i++)
			sum1 += vec1[i].w_float;

		mean = sum/x->x_array_points;
		mean1 = sum1/array1pts;

		for(i=0; i<x->x_array_points; i++)
			vec_centered[i] = x->x_vec[i].w_float-mean;

		for(i=0; i<array1pts; i++)
			vec1_centered[i] = vec1[i].w_float-mean1;

		for(i=0; i<x->x_array_points; i++)
			std += vec_centered[i]*vec_centered[i];

		for(i=0; i<array1pts; i++)
			std1 += vec1_centered[i]*vec1_centered[i];

		std = sqrt(std/(x->x_array_points-1));
		std1 = sqrt(std1/(array1pts-1));

		for(i=0; i<array1pts; i++)
			if(i>x->x_array_points-1)
				break;
			else
				corr += vec_centered[i]*vec1_centered[i];

		corr /=(array1pts-1);
		corr = corr/(std*std1);

		outlet_float(x->x_info, corr);

		// free local memory
		t_freebytes(vec_centered, x->x_array_points * sizeof(t_float));
		t_freebytes(vec1_centered, array1pts * sizeof(t_float));
	}
}


static void tabletool_abs(t_tabletool *x)
{
	int i;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float < 0)
				x->x_vec[i].w_float *= -1;

		garray_redraw(a);
 	}
}


static void tabletool_sum(t_tabletool *x)
{
	int i;
	t_float sum;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		sum = 0.0;

		for(i=0; i<x->x_array_points; i++)
			sum += x->x_vec[i].w_float;

		outlet_float(x->x_info, sum);
 	}
}


static void tabletool_mean(t_tabletool *x)
{
	int i;
	t_float mean;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		mean = 0.0;

		for(i=0; i<x->x_array_points; i++)
			mean += x->x_vec[i].w_float;

		mean /= x->x_array_points;

		outlet_float(x->x_info, mean);
 	}
}


static void tabletool_geomean(t_tabletool *x)
{
	int i;
	double num_points_recip, mean, *nth_roots;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		nth_roots = (double *)t_getbytes(x->x_array_points*sizeof(double));

		mean = 1.0;
		num_points_recip = 1.0/x->x_array_points;

		// take the nth roots first so as not to lose data.
		for(i=0; i<x->x_array_points; i++)
			nth_roots[i] = pow(x->x_vec[i].w_float, num_points_recip);

		// take the product of nth roots
		for(i=0; i<x->x_array_points; i++)
			mean *= nth_roots[i];

		outlet_float(x->x_info, mean);

		// free local memory
		t_freebytes(nth_roots, x->x_array_points*sizeof(double));
 	}
}


static void tabletool_std(t_tabletool *x)
{
	int i;
	t_float *tablevals, sum, mean, std;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		tablevals = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		for(i=0; i<x->x_array_points; i++)
			tablevals[i] = x->x_vec[i].w_float;

		sum = 0.0;

		for(i=0; i<x->x_array_points; i++)
			sum += tablevals[i];

		mean = sum/x->x_array_points;

		sum = 0.0;

		// center & square the data
		for(i=0; i<x->x_array_points; i++)
		{
			tablevals[i] -= mean;
			tablevals[i] *= tablevals[i];
			sum += tablevals[i];
		}

		std = sum/(x->x_array_points-1.0);
		std = sqrt(std);

		outlet_float(x->x_info, std);

		// free local memory
		t_freebytes(tablevals, x->x_array_points * sizeof(t_float));
 	}
}


static void tabletool_normalize(t_tabletool *x, t_float min, t_float max)
{
	int i;
	t_float range_data, range_final, smallest, largest;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		smallest = FLT_MAX;

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float < smallest)
				smallest = x->x_vec[i].w_float;

		largest = smallest;

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float > largest)
				largest = x->x_vec[i].w_float;

		range_final = max - min;
		range_data = largest - smallest;

		if(range_final <= 0)
			range_final = 1;

		if(range_data <= 0)
			range_data = 1;

		range_data = 1.0/range_data;

		// shift everything >= 0
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float -= smallest;

		// scale to 0-1 range
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float *= range_data;

		// scale to requested range
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float *= range_final;

		// offset downward according to min
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float += min;

		garray_redraw(a);
 	}
}


static void tabletool_normalize_sum(t_tabletool *x)
{
	int i;
	t_float sum, smallest;
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		smallest = FLT_MAX;

		for(i=0; i<x->x_array_points; i++)
			if(x->x_vec[i].w_float < smallest)
				smallest = x->x_vec[i].w_float;

		// shift everything >= 0
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float -= smallest;

		sum = 0.0;

		for(i=0; i<x->x_array_points; i++)
			sum += x->x_vec[i].w_float;

		sum = 1.0/sum;

		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float *= sum;

		garray_redraw(a);
 	}
}


static void tabletool_set(t_tabletool *x, t_symbol *s)
{
	t_garray *a;
	int old_array_points;

	old_array_points = x->x_array_points;

	if(!(a = (t_garray *)pd_findbyclass(s, garray_class)))
		pd_error(x, "%s: no such array", s->s_name);
	else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
		pd_error(x, "%s: bad template for tabletool", s->s_name);
	else
	{
	    x->x_arrayname = s;
	}
}


static void tabletool_copy(t_tabletool *x, t_symbol *source)
{
	int i, sourcepts;
	t_garray *a, *b;
	t_word *sourcevec;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{

		if(!(b = (t_garray *)pd_findbyclass(source, garray_class)))
		{
			pd_error(x, "%s: no such array", source->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &sourcepts, &sourcevec))
		{
			pd_error(x, "%s: bad template for tabletool", source->s_name);
			return;
		}

		for(i=0; i<sourcepts; i++)
			if(i > x->x_array_points-1)
				break;
			else
				x->x_vec[i].w_float = sourcevec[i].w_float;

		for(; i<x->x_array_points; i++)
			x->x_vec[i].w_float = 0.0;

		garray_redraw(a);
	}
}


static void tabletool_copy_range(t_tabletool *x, t_symbol *s, int argc, t_atom *argv)
{
	int i, j, sourcepts, ts, ss, se;
	t_garray *a, *b;
	t_word *sourcevec;
	t_symbol *source;

	argc=argc;
	s=s;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		ts = atom_getfloat(argv+0);
		source = atom_getsymbol(argv+1);
		ss = atom_getfloat(argv+2);
		se = atom_getfloat(argv+3);

		if(!(b = (t_garray *)pd_findbyclass(source, garray_class)))
		{
			pd_error(x, "%s: no such array", source->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, &sourcepts, &sourcevec))
		{
			pd_error(x, "%s: bad template for tabletool", source->s_name);
			return;
		}

		if(ts<0)
			ts = 0;

		if(ss<0)
			ss = 0;

		if(se>sourcepts-1)
			se = sourcepts-1;

		for(i=ts, j=ss; j<=se; i++, j++)
			if(i > x->x_array_points-1)
				break;
			else
				x->x_vec[i].w_float = sourcevec[j].w_float;

		garray_redraw(a);
	}
}


static void tabletool_blackman(t_tabletool *x, t_symbol *s)
{
	t_garray *a;
	int i;

	s=s;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float = 0.42 - (0.5 * cos(2*M_PI*i/x->x_array_points)) + (0.08 * cos(4*M_PI*i/x->x_array_points));

		garray_redraw(a);
	}
}


static void tabletool_cosine(t_tabletool *x, t_symbol *s)
{
	t_garray *a;
	int i;

    s=s;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float = sin(M_PI*i/x->x_array_points);

		garray_redraw(a);
	}
}


static void tabletool_hamming(t_tabletool *x, t_symbol *s)
{
	t_garray *a;
	int i;

    s=s;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float = 0.5 - (0.46 * cos(2*M_PI*i/x->x_array_points));

		garray_redraw(a);
	}
}


static void tabletool_hann(t_tabletool *x, t_symbol *s)
{
	t_garray *a;
	int i;

    s=s;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		for(i=0; i<x->x_array_points; i++)
			x->x_vec[i].w_float = 0.5 * (1 - cos(2*M_PI*i/x->x_array_points));

		garray_redraw(a);
	}
}


static void tabletool_randFill(t_tabletool *x, t_float min, t_float max)
{
	t_garray *a;
	int i;
	t_float range;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		range = max - min;

		for(i=0; i<x->x_array_points; i++)
		{
			x->x_vec[i].w_float = rand();
			x->x_vec[i].w_float /= RAND_MAX;
			x->x_vec[i].w_float *= range;
			x->x_vec[i].w_float += min;
		}

		garray_redraw(a);
 	}
}


static void tabletool_peaks(t_tabletool *x, t_float min, t_float max)
{
	t_garray *a;
	int i, peakFlag, peakMaxIdx;
	t_float peakMax, tmp;
	t_atom *index_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		index_out = (t_atom *)t_getbytes(sizeof(t_atom));

		if(min > max)
		{
			tmp = min;
			min = max;
			max = tmp;
		}

		peakFlag = 0;
		peakMax = 0;
		peakMaxIdx = 0;

		for(i=0; i<x->x_array_points-1; i++)
		{
			if(x->x_vec[i].w_float > max)
				peakFlag = 1;

			if(peakFlag && (x->x_vec[i].w_float > peakMax))
			{
				peakMax = x->x_vec[i].w_float;
				peakMaxIdx = i;
			}
			else if(peakFlag && (x->x_vec[i].w_float < min))
			{

				SETFLOAT(index_out, peakMaxIdx);
				outlet_list(x->x_list, 0, 1, index_out);
				outlet_float(x->x_info, peakMax);

				peakFlag = 0;
				peakMax = 0;
				peakMaxIdx = 0;
			}
		}

		// free local memory
		t_freebytes(index_out, sizeof(t_atom));
	}
}


static void tabletool_store(t_tabletool *x)
{
	t_garray *a;
	int i;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		x->original_data = (t_float *)t_getbytes(x->x_array_points*sizeof(t_float));

		// load table data
		for(i=0; i<x->x_array_points; i++)
			x->original_data[i] = x->x_vec[i].w_float;

		x->stored_flag = 1;
	}
}


static void tabletool_restore(t_tabletool *x)
{
	t_garray *a;
	int i;

	if(x->stored_flag)
	{
		if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
			pd_error(x, "%s: no such array", x->x_arrayname->s_name);
		else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
			pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
		else
		{
			// load original data
			for(i=0; i<x->x_array_points; i++)
				x->x_vec[i].w_float = x->original_data[i];

			garray_redraw(a);
		}
	}
	else
		error("tabletool: table not previously stored");
}


static void tabletool_wipe(t_tabletool *x)
{
	if(x->stored_flag)
	{
		t_freebytes(x->original_data, x->x_array_points*sizeof(t_float));
		x->stored_flag = 0;
	}
}


static void tabletool_change(t_tabletool *x)
{
	int i;
	t_garray *a;
	t_atom *index_out;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else
	{
		if(x->stored_flag)
		{
			index_out = (t_atom *)t_getbytes(sizeof(t_atom));

			for(i=0; i<x->x_array_points; i++)
			{
				if(x->x_vec[i].w_float != x->original_data[i])
				{
					SETFLOAT(index_out, i);
					outlet_list(x->x_list, 0, 1, index_out);
					outlet_float(x->x_info, x->x_vec[i].w_float);
					x->original_data[i] = x->x_vec[i].w_float;
				}
			}

			// free local memory
			t_freebytes(index_out, sizeof(t_atom));
		}
		else
			tabletool_store(x);
 	}
}


static void *tabletool_new(t_symbol *s)
{
    t_tabletool *x = (t_tabletool *)pd_new(tabletool_class);

	x->x_info = outlet_new(&x->x_obj, &s_float);
	x->x_list = outlet_new(&x->x_obj, gensym("list"));

	x->x_arrayname = s;

	x->stored_flag = 0;
	x->rand_state = (int)clock_getlogicaltime(); // seed with (int) logical time

    return (x);
}


static void tabletool_free(t_tabletool *x)
{
	if(x->stored_flag)
	    t_freebytes(x->original_data, x->x_array_points*sizeof(t_float));
}


void tabletool_setup(void)
{
    tabletool_class =
    class_new(
    	gensym("tabletool"),
    	(t_newmethod)tabletool_new,
    	(t_method)tabletool_free,
        sizeof(t_tabletool),
        CLASS_DEFAULT,
        A_DEFSYMBOL,
		0
    );

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_dump,
		gensym("dump"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_dumpRange,
		gensym("dump_range"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);
	
	class_addmethod(
		tabletool_class,
		(t_method)tabletool_size,
		gensym("size"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_min,
		gensym("min"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_max,
		gensym("max"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_nearest,
		gensym("nearest"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_equals,
		gensym("equals"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_greater,
		gensym("greater"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_less,
		gensym("less"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_between,
		gensym("between"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_choose,
		gensym("choose"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_const,
		gensym("const"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_series,
		gensym("series"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_random_walk,
		gensym("random_walk"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_offset,
		gensym("offset"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_shift,
		gensym("shift"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_shift0,
		gensym("shift0"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_invert,
		gensym("invert"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_reverse,
		gensym("reverse"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_remove,
		gensym("remove"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_insert,
		gensym("insert"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_smooth,
		gensym("smooth"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_lace,
		gensym("lace"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_concatenate,
		gensym("concatenate"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_scramble,
		gensym("scramble"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_sort,
		gensym("sort"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_sort_range,
		gensym("sort_range"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_swap,
		gensym("swap"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_integrate,
		gensym("integrate"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_differentiate,
		gensym("differentiate"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_add,
		gensym("add"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_add_range,
		gensym("add_range"),
		A_GIMME,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_subtract,
		gensym("subtract"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_multiply,
		gensym("multiply"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_divide,
		gensym("divide"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_dot,
		gensym("dot"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_euclid,
		gensym("euclid"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_taxi,
		gensym("taxi"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_corr,
		gensym("corr"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_abs,
		gensym("abs"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_sum,
		gensym("sum"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_mean,
		gensym("mean"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_geomean,
		gensym("geomean"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_std,
		gensym("std"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_scale,
		gensym("scale"),
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_normalize,
		gensym("normalize"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_normalize_sum,
		gensym("normalize_sum"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_set,
		gensym("set"),
		A_SYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_copy,
		gensym("copy"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_copy_range,
		gensym("copy_range"),
		A_GIMME,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_blackman,
		gensym("blackman"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_cosine,
		gensym("cosine"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_hamming,
		gensym("hamming"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_hann,
		gensym("hann"),
		A_DEFSYMBOL,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_randFill,
		gensym("randFill"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_peaks,
		gensym("peaks"),
		A_DEFFLOAT,
		A_DEFFLOAT,
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_store,
		gensym("store"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_restore,
		gensym("restore"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_wipe,
		gensym("wipe"),
		0
	);

	class_addmethod(
		tabletool_class,
		(t_method)tabletool_change,
		gensym("change"),
		0
	);
}

