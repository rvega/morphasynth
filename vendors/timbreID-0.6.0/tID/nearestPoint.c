/*

nearestPoint - Compare an incoming coordinate with a database of coordinates and output the nearest point along with its distance from the input.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.0.4, December 21, 2011

¥ 0.0.4 uses tIDLib.h for global includes and changes float to t_float
¥ 0.0.3 as part of the timbreID-0.5 update, this gets rid of unnecessary getbytes(0) calls and uses memset() or memcpy() where possible. also fixes an improper memory free in nearestPoint_free and _clear
¥ 0.0.2 declares all functions except _setup static

*/

#include "tIDLib.h"

static t_class *nearestPoint_class;
    	
typedef struct coordinate
{
    t_float *coordinate;
} t_coordinate;


typedef struct _nearestPoint
{
    t_object x_obj;
    t_coordinate *x_coordinates;
	t_float *x_input;

    int dimensions;
    int numPoints;
    
    t_outlet *nearest;
    t_outlet *nearestDist;
    
} t_nearestPoint;


/* ---------------- utility functions ---------------------- */

static t_float nearestPoint_euclid(int n, t_float *v1, t_float *v2)
{
	int i;
	t_float sum, dist;
	
	sum=dist=0;

	for(i=0; i<n; i++)
	{
		dist = v1[i] - v2[i];
		sum += dist*dist;
	}

	sum = sqrt(sum);
	
	return(sum);
}

/* ---------------- END utility functions ---------------------- */




/* ------------------------ nearestPoint -------------------------------- */

static void nearestPoint_add(t_nearestPoint *x, t_symbol *s, int argc, t_atom *argv)
{
	int i, pointIdx, dimensions;

	pointIdx = x->numPoints;
	dimensions = argc;
	s=s; // to get rid of 'unused variable' warning

	if(dimensions == x->dimensions)
	{
		x->x_coordinates = (t_coordinate *)t_resizebytes(x->x_coordinates, x->numPoints * sizeof(t_coordinate), (x->numPoints+1) * sizeof(t_coordinate));
		
		x->x_coordinates[pointIdx].coordinate = (t_float *)t_getbytes(dimensions*sizeof(t_float));
		
		x->numPoints++;
				
		for(i=0; i<dimensions; i++)
			x->x_coordinates[pointIdx].coordinate[i] = atom_getfloat(argv+i);
	}
	else
		error("nearestPoint: dimensionality mismatch.");
}


static void nearestPoint_nearest(t_nearestPoint *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float sum, best;
    int i, dimensions, nearest;
	s=s; // to get rid of 'unused variable' warning

	if(x->numPoints)
	{		
		dimensions = argc;
		
		if(dimensions == x->dimensions)
		{
			for(i=0; i<dimensions; i++)
				x->x_input[i] = atom_getfloat(argv+i);		
				
			nearest = 0;
			best = FLT_MAX;
			
			for(i=0; i<x->numPoints; i++)
			{
				sum = 0;		
				sum = nearestPoint_euclid(dimensions, x->x_input, x->x_coordinates[i].coordinate);

				if(sum < best)
				{
					nearest = i;
					best = sum;
				}
			};
			
			outlet_float(x->nearestDist, best);
			outlet_float(x->nearest, nearest);
		}
		else
			error("nearestPoint: dimensionality mismatch.");
    }
}


static void nearestPoint_dimensions(t_nearestPoint *x, t_floatarg dim)
{
	if(x->numPoints > 0)
	{
		error("nearestPoint: clear all coordinates before changing dimensionality.");
		return;
	}
	else if(dim<=0)
		x->dimensions = 1;
	else
		x->dimensions = dim;

	post("dimensionality: %i.", x->dimensions);
}


static void nearestPoint_clear(t_nearestPoint *x)
{
	int i;
	
	for(i=0; i<x->numPoints; i++)
		t_freebytes(x->x_coordinates[i].coordinate, x->dimensions*sizeof(t_float));
		
	x->x_coordinates = (t_coordinate *)t_resizebytes(x->x_coordinates, x->numPoints*sizeof(t_coordinate), 0);

	x->numPoints = 0;
}


static void *nearestPoint_new(t_float dim)
{
    t_nearestPoint *x = (t_nearestPoint *)pd_new(nearestPoint_class);
	int i;
	
    x->nearest = outlet_new(&x->x_obj, &s_float);
    x->nearestDist = outlet_new(&x->x_obj, &s_float);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("nearest"));
    	
	if(dim)
		x->dimensions = dim;
	else
		x->dimensions = 2;

    x->numPoints = 0;

    x->x_coordinates = (t_coordinate *)t_getbytes(x->numPoints);

	// resize feature input buffer to default dimensions
    x->x_input = (t_float *)t_getbytes(x->dimensions*sizeof(t_float));

    // initialize feature input buffer
	for(i=0; i<x->dimensions; i++)
		x->x_input[i] = 0.0;

	post("nearestPoint: dimensionality: %i.", x->dimensions);

    return (x);
}


static void nearestPoint_free(t_nearestPoint *x)
{
	int i;
		
	for(i=0; i<x->numPoints; i++)
		t_freebytes(x->x_coordinates[i].coordinate, x->dimensions*sizeof(t_float));
	
	t_freebytes(x->x_coordinates, x->numPoints*sizeof(t_coordinate));
	
	t_freebytes(x->x_input, x->dimensions*sizeof(t_float));
}


void nearestPoint_setup(void)
{
    nearestPoint_class = 
    class_new(
    	gensym("nearestPoint"),
    	(t_newmethod)nearestPoint_new,
    	(t_method)nearestPoint_free,
        sizeof(t_nearestPoint),
        CLASS_DEFAULT,
        A_DEFFLOAT,
		0
    );

	class_addlist(
		nearestPoint_class,
		(t_method)nearestPoint_add
	);

	class_addmethod(
		nearestPoint_class, 
        (t_method)nearestPoint_nearest,
		gensym("nearest"),
        A_GIMME,
		0
	);

	class_addmethod(
		nearestPoint_class, 
        (t_method)nearestPoint_dimensions, 
		gensym("dimensions"),
		A_DEFFLOAT,
		0
	);
	
	class_addmethod(
		nearestPoint_class, 
        (t_method)nearestPoint_clear,
		gensym("clear"),
		0
	);
}