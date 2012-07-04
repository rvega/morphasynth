#pragma once

#include <iostream>
#include <string.h>

#include "SineWave.h"
#include "ringbuffer.h"
#include "Event.h"
#include "DSPOscillator.h"
#include "ADSR.h"
#include "DSPNoiseWithLevel.h"
#include "DSPLoPass.h"
#include "DSPHiPass.h"

using namespace stk;

class DSPSynthesizer{
   private:
      unsigned int sampleRate;
      StkFloat* outputBuffer;
      jack_ringbuffer_t* guiEventsBuffer;

      Event event;

      DSPNoiseWithLevel* noise;
      DSPOscillator* oscillator1;
      ADSR* envelope;
      DSPLoPass* lopass;
      DSPHiPass* hipass;

      // Some intermediate signals:
      StkFloat out;
      StkFloat s1;

      void processEvent(Event* event);

   public:
      DSPSynthesizer();
      ~DSPSynthesizer();

      void setSampleRate(unsigned int sampleRate);
      int process(void *outBuffer, void *inBuffer, unsigned int bufferSize);
      void addGUIEvent(Event event);
      void addMidiEvent(Event event);
};

