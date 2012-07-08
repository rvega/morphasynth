/**
 * Morphasynth
 * 
 * Copyright (C) 2012 Rafael Vega González
 * Copyright (C) 2012 Daniel Gómez Marín
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DSPLoPass.h"
#include <math.h>

DSPLoPass::DSPLoPass(): 
   gain(1), 
   frequency(200.0), 
   resonance(0.8),
   out(0.0)      
{
      //Constructor
		fslider0 = 0.9f;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		fslider1 = 1e+03f;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
}

DSPLoPass::~DSPLoPass(){
}

void DSPLoPass::setSampleRate(unsigned int sr){
   if(sr > 192000.0) sr = 192000.0;
   else if(sr < 1.0) sr = 1.0;
   sampleRate = sr;

   fConst0 = (3.141592653589793f / faustmin(192000, faustmax(1, sampleRate)));
}

void DSPLoPass::setGain(StkFloat g){
   if(g > 2.0) g = 2.0;
   else if(g < 0.0) g = 0.0;
   gain = g;
}

void DSPLoPass::setFrequency(StkFloat freq){
   if(freq > 22000.0) freq=22000.0;
   else if(freq < 0) freq=0.0;
   frequency = freq;
}

void DSPLoPass::setResonance(StkFloat res){
   if(res > 1.0) res = 1.0;
   else if(res < 0.0) res = 0.0;
   resonance = res;
}
