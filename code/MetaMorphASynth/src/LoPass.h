#pragma once

#include "Stk.h"
#include "FaustDefinitions.h"
#include "SineWave.h"
#include "BlitSquare.h"
#include "BlitSaw.h"

using namespace stk;

class LoPass{
   protected:
      StkFloat gain;
      StkFloat frequency;
      StkFloat resonance;
      unsigned int sampleRate; 
      StkFloat out;

      // DSP Variables
      FAUSTFLOAT 	fslider0;
      float 	fRec0[2];
      FAUSTFLOAT 	fslider1;
      float 	fRec1[2];
      float 	fConst0;
      float 	fRec6[2];
      float 	fRec4[2];
      float 	fRec7[2];
      float 	fRec2[2];

   public:
      LoPass();
      ~LoPass();

      void setGain(StkFloat g);
      void setFrequency(StkFloat freq);
      void setResonance(StkFloat res);
      void setSampleRate(unsigned int sr);
      inline StkFloat tick(StkFloat input);
};

inline StkFloat LoPass::tick(StkFloat input){
		float 	fSlow0 = (0.0010000000000000009f * resonance);
		float 	fSlow1 = (0.0010000000000000009f * frequency);
		FAUSTFLOAT* input0 = &input;
		FAUSTFLOAT* output0 = &out;
		// for (int i=0; i<count; i++) {
			fRec0[0] = (fSlow0 + (0.999f * fRec0[1]));
			float fTemp0 = (1.9980000000000002f * fRec0[0]);
			fRec1[0] = (fSlow1 + (0.999f * fRec1[1]));
			float fTemp1 = tanf((fConst0 * faustmax(fRec1[0], 20)));
			float fTemp2 = (2 - fTemp0);
			float fTemp3 = (1.0f / fTemp1);
			float fTemp4 = faustpower<2>((1.412799348810722f * fRec0[0]));
			float fTemp5 = ((1 + (fTemp4 + ((fTemp3 + fTemp2) / fTemp1))) - fTemp0);
			float fTemp6 = (((1 + (fTemp4 + ((fTemp3 - fTemp2) / fTemp1))) - fTemp0) / fTemp5);
			float fTemp7 = faustmax(-0.9999f, faustmin(0.9999f, fTemp6));
			float fTemp8 = (1 - faustpower<2>(fTemp7));
			float fTemp9 = sqrtf(fTemp8);
			float fTemp10 = (1.0f / faustpower<2>(fTemp1));
			float fTemp11 = ((1 + fTemp4) - (fTemp10 + fTemp0));
			float fTemp12 = faustmax(-0.9999f, faustmin(0.9999f, (2 * (fTemp11 / (fTemp5 * (1 + fTemp6))))));
			float fTemp13 = (1 - faustpower<2>(fTemp12));
			float fTemp14 = (2.0f - (2.0f * (fTemp11 / fTemp5)));
			float fTemp15 = (2 + fTemp0);
			float fTemp16 = (fTemp0 + fTemp4);
			float fTemp17 = (1 + (fTemp16 + ((fTemp3 + fTemp15) / fTemp1)));
			float fTemp18 = ((1 + (fTemp16 + ((fTemp3 - fTemp15) / fTemp1))) / fTemp17);
			float fTemp19 = faustmax(-0.9999f, faustmin(0.9999f, fTemp18));
			float fTemp20 = (1 - faustpower<2>(fTemp19));
			float fTemp21 = sqrtf(fTemp20);
			float fTemp22 = ((1 + fTemp16) - fTemp10);
			float fTemp23 = faustmax(-0.9999f, faustmin(0.9999f, (2 * (fTemp22 / (fTemp17 * (1 + fTemp18))))));
			float fTemp24 = (1 - faustpower<2>(fTemp23));
			float fTemp25 = (2.0f - (2.0f * (fTemp22 / fTemp17)));
			float fTemp26 = sqrtf(faustmax(0, fTemp20));
			float fTemp27 = (float)*input0;
			float fTemp28 = ((fTemp27 * fTemp26) + ((0 - fTemp19) * fRec4[1]));
			float fTemp29 = sqrtf(faustmax(0, fTemp24));
			fRec6[0] = ((fTemp29 * fTemp28) + ((0 - fTemp23) * fRec6[1]));
			fRec4[0] = ((fTemp29 * fRec6[1]) + (fTemp23 * fTemp28));
			float 	fRec5 = fRec6[0];
			float fTemp30 = ((((fTemp26 * fRec4[1]) + (fTemp27 * fTemp19)) + ((fRec4[0] * fTemp25) / fTemp21)) + ((fRec5 * ((1.0f - fTemp18) - (fTemp23 * fTemp25))) / (sqrtf(fTemp24) * fTemp21)));
			float fTemp31 = sqrtf(faustmax(0, fTemp8));
			float fTemp32 = (((fTemp31 * fTemp30) / fTemp17) + ((0 - fTemp7) * fRec2[1]));
			float fTemp33 = sqrtf(faustmax(0, fTemp13));
			fRec7[0] = ((fTemp33 * fTemp32) + ((0 - fTemp12) * fRec7[1]));
			fRec2[0] = ((fTemp33 * fRec7[1]) + (fTemp12 * fTemp32));
			float 	fRec3 = fRec7[0];
			*output0 = (FAUSTFLOAT)(((((fTemp31 * fRec2[1]) + ((fTemp7 * fTemp30) / fTemp17)) + ((fRec2[0] * fTemp14) / fTemp9)) + ((fRec3 * ((1.0f - fTemp6) - (fTemp12 * fTemp14))) / (sqrtf(fTemp13) * fTemp9))) / fTemp5);
			// post processing
			fRec2[1] = fRec2[0];
			fRec7[1] = fRec7[0];
			fRec4[1] = fRec4[0];
			fRec6[1] = fRec6[0];
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
		// }

   return out;
}
