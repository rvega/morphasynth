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

#include "DSPHiPass.h"
#include <math.h>

DSPHiPass::DSPHiPass(): 
   gain(1), 
   frequency(200.0), 
   q(0.07),
   out(0.0)      
{
      //Constructor
		fslider0 = 0.07f;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		fslider1 = 0.0f;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<3; i++) fVec0[i] = 0;
		for (int i=0; i<3; i++) fRec0[i] = 0;
}

DSPHiPass::~DSPHiPass(){
}

void DSPHiPass::setSampleRate(unsigned int sr){
   if(sr > 192000.0) sr = 192000.0;
   else if(sr < 1.0) sr = 1.0;
   sampleRate = sr;

   fConst0 = (6.283185307179586f / sampleRate);
}

void DSPHiPass::setGain(StkFloat g){
   if(g > 2.0) g = 2.0;
   else if(g < 0.0) g = 0.0;
   gain = g;
}

void DSPHiPass::setFrequency(StkFloat freq){
   if(freq > 22000.0) freq=22000.0;
   else if(freq < 10.0) freq=10.0;
   frequency = freq;
}

void DSPHiPass::setResonance(StkFloat res){
   // TODO: this kind of value mapping should be handled in the GUI, we need a more robust
   //       slider class that takes care of this sort of thing and also allows for non-linear 
   //       parameter tweaking. 
   //       Here's a generic range conversion function. low1 and high1 are in the same range of the original value.
   //           function map($value, $low1, $high1, $low2, $high2) {
   //               return ($value / ($high1 - $low1)) * ($high2 - $low2) + $low2;
   //           }

   // Resonance between 0 and 1 but we're really setting q which has a range of 0.07 to 30
   if(res > 1.0) res = 1.0;
   else if(res < 0.0) res = 0.0;

   q = (res / (1.0 - 0.0)) * (30.0 - 0.07) + 0.07;  
}
