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

#pragma once

#include "Stk.h"
#include "SineWave.h"
#include "BlitSquare.h"
#include "BlitSaw.h"
#include "FaustDefinitions.h"

using namespace stk;


class HiPass{
   protected:
      StkFloat gain;
      StkFloat frequency;
      StkFloat q;
      unsigned int sampleRate; 
      StkFloat out;

      // DSP Variables
      FAUSTFLOAT 	fslider0;
      float 	fRec1[2];
      FAUSTFLOAT 	fslider1;
      float 	fRec2[2];
      float 	fConst0;
      float 	fVec0[3];
      float 	fRec0[3];

   public:
      HiPass();
      ~HiPass();

      void setGain(StkFloat g);
      void setFrequency(StkFloat freq);
      void setResonance(StkFloat res);
      void setSampleRate(unsigned int sr);
      inline StkFloat tick(StkFloat input);
};

inline StkFloat HiPass::tick(StkFloat input){
		float 	fSlow0 = (0.0010000000000000009f * q);
		float 	fSlow1 = (0.0010000000000000009f * frequency);
		FAUSTFLOAT* input0 = &input;
		FAUSTFLOAT* output0 = &out;
		// for (int i=0; i<count; i++) {
			fRec1[0] = (fSlow0 + (0.999f * fRec1[1]));
			fRec2[0] = (fSlow1 + (0.999f * fRec2[1]));
			float fTemp0 = (fConst0 * faustmax(0, fRec2[0]));
			float fTemp1 = (0.5f * (sinf(fTemp0) / faustmax(0.001f, fRec1[0])));
			float fTemp2 = cosf(fTemp0);
			float fTemp3 = (1 + fTemp2);
			float fTemp4 = (float)*input0;
			fVec0[0] = fTemp4;
			fRec0[0] = (((((fVec0[1] * (0 - fTemp3)) + (0.5f * (fVec0[0] * fTemp3))) + (0.5f * (fVec0[2] * fTemp3))) + ((fRec0[2] * (fTemp1 - 1)) + (2 * (fTemp2 * fRec0[1])))) / (1 + fTemp1));
			*output0 = (FAUSTFLOAT)fRec0[0];

			// post processing
			fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
			fVec0[2] = fVec0[1]; fVec0[1] = fVec0[0];
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
		// }

   return out;
}
