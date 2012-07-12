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
   sampleRate = sr;

   fConst0 = (3.141592653589793f / faustmin(192000, faustmax(1, sampleRate)));
}

void DSPLoPass::setGain(StkFloat g){
   gain = g;
}

void DSPLoPass::setFrequency(StkFloat freq){
   frequency = freq;
}

void DSPLoPass::setResonance(StkFloat res){
   resonance = res;
}
