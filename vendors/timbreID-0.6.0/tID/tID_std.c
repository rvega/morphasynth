	/*

tID_std - Calculate standard deviation of a list of numbers.

Copyright 2009 William Brent

This file is part of timbreID.

timbreID is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

timbreID is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


version 0.0.4, December 21, 2011

• 0.0.4 uses tIDLib.h for global includes and changes float to t_float
• 0.0.3 as part of the timbreID-0.5 update, this gets rid of unnecessary getbytes(0) calls and uses memset() or memcpy() where possible
• 0.0.2 declares all functions except _setup static

*/

#include "tIDLib.h"

static t_class *tID_std_class;

typedef struct _tID_std
{
    t_object x_obj;
    t_outlet *std;
    
} t_tID_std;


/* ------------------------ tID_std -------------------------------- */

static void tID_std_calculate(t_tID_std *x, t_symbol *s, int argc, t_atom *argv)
{
	t_float n, sum, mean, std, *input;
    int i;
	s=s;
	
	n = argc;
	
	if(n <= 1)
		error("tID_std: too few elements in list.");
	else
	{
		// create local memory
		input = (t_float *)t_getbytes(n*sizeof(t_float));

		for(i=0; i<n; i++)
			input[i] = atom_getfloat(argv+i);
	
		sum = 0.0;
	
		for(i=0; i<n; i++)
			sum += input[i];
	
		mean = sum/n;
		
		sum = 0.0;
		
		// center & square the data
		for(i=0; i<n; i++)
		{
			input[i] -= mean;
			input[i] *= input[i];
			sum += input[i];
		}
		
		std = sum/(n-1.0);
		std = sqrt(std);
			
		outlet_float(x->std, std);

		// free local memory
		t_freebytes(input, n*sizeof(t_float));
	}	
}

static void *tID_std_new(void)
{	
    t_tID_std *x = (t_tID_std *)pd_new(tID_std_class);
    x->std = outlet_new(&x->x_obj, &s_float);
    
    return (x);
}

void tID_std_setup(void)
{
    tID_std_class = 
    class_new(
    	gensym("tID_std"),
    	(t_newmethod)tID_std_new,
    	0,
        sizeof(t_tID_std),
        CLASS_DEFAULT, 
		0
    );

	class_addlist(
		tID_std_class,
		(t_method)tID_std_calculate
	);
}