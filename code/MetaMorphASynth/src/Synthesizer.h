#pragma once

#include <iostream>
#include <string.h>

#include "SineWave.h"
#include "ringbuffer.h"
#include "Event.h"
#include "Oscillator.h"
#include "ADSR.h"
#include "NoiseWithLevel.h"
#include "LoPass.h"
#include "HiPass.h"

using namespace stk;

class Synthesizer{
   protected:
      unsigned int sampleRate;
      StkFloat* outputBuffer;
      jack_ringbuffer_t *eventsBuffer;

      Event event;

      NoiseWithLevel* noise;
      Oscillator* oscillator1;
      ADSR* envelope;
      LoPass* lopass;
      HiPass* hipass;

      // Some intermediate signals:
      StkFloat out;
      StkFloat s1;

      void processEvent(Event* event);

   public:
      Synthesizer();
      ~Synthesizer();

      void setSampleRate(unsigned int sampleRate);
      int process(void *outBuffer, void *inBuffer, unsigned int bufferSize);
      void pushEvent(Event* event);
};

