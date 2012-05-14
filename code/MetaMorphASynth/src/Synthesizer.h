#pragma once

#include <iostream>
#include "SineWave.h"

using namespace stk;

class Synthesizer{
   protected:
      unsigned int sampleRate;
      StkFloat* outputBuffer;
      StkFloat out;
      SineWave* sine;

      StkFloat freq;
   public:
      Synthesizer();
      ~Synthesizer();

      void setSampleRate(unsigned int sampleRate);
      
      int process(void *outBuffer, void *inBuffer, unsigned int bufferSize);
};

