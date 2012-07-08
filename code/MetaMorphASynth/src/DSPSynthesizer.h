#pragma once

#include <iostream>
#include <string.h>

#include "SineWave.h"
#include "ringbuffer.h"
#include "GuiEvent.h"
#include "MidiEvent.h"
#include "MidiUtilities.h"
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

      // Event buffers
      jack_ringbuffer_t* guiEventsBuffer;
      jack_ringbuffer_t* midiEventsBuffer;

      MidiUtilities* midiUtils;

      // DSP Blocks:
      DSPNoiseWithLevel* noise;
      DSPOscillator* oscillator1;
      ADSR* envelope;
      DSPLoPass* lopass;
      DSPHiPass* hipass;
      
      // MidiEvent being processed
      MidiEvent midiEvent; 

      // Midi number of the note being played
      short int currentNote;
      
      // GuiEvent being processed
      GuiEvent guiEvent; 

      // Some intermediate signals:
      StkFloat out;
      StkFloat s1;

      void processGuiEvent(GuiEvent* event);
      void processMidiEvent(MidiEvent* event);

   public:
      DSPSynthesizer();
      ~DSPSynthesizer();

      void setSampleRate(unsigned int sampleRate);
      int process(void *outBuffer, void *inBuffer, unsigned int bufferSize);
      void addGUIEvent(GuiEvent event);
      void addMidiEvent(MidiEvent event);
};

