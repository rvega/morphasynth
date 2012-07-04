#pragma once

#include "AudioIO.h"
#include "DSPSynthesizer.h"
#include "GUI.h"
#include "ringbuffer.h"
#include "Event.h"
#include "Parameter.h"

class Controller{
	public:
      Controller();
      ~Controller();

		void addMidiEvent(Event e);
      void addGUIEvent(Event e);

   private:
      std::map<Parameter, float> parameters;
      GUI* gui;
      DSPSynthesizer* synth;
      AudioIO* audioIO;
      // jack_ringbuffer_t* midiEvents;

      void setInitialParameters();
};
