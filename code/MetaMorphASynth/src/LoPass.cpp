#include "LoPass.h"
#include <math.h>

LoPass::LoPass(): 
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

LoPass::~LoPass(){
}

void LoPass::setSampleRate(unsigned int sr){
   if(sr > 192000.0) sr = 192000.0;
   else if(sr < 1.0) sr = 1.0;
   sampleRate = sr;

   fConst0 = (3.141592653589793f / faustmin(192000, faustmax(1, sampleRate)));
}

void LoPass::setGain(StkFloat g){
   if(g > 2.0) g = 2.0;
   else if(g < 0.0) g = 0.0;
   gain = g;
}

void LoPass::setFrequency(StkFloat freq){
   if(freq > 22000.0) freq=22000.0;
   else if(freq < 0) freq=0.0;
   frequency = freq;
}

void LoPass::setResonance(StkFloat res){
   if(res > 1.0) res = 1.0;
   else if(res < 0.0) res = 0.0;
   resonance = res;
}
