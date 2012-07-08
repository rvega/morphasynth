#pragma once

#include "AudioIO.h"
#include "DSPSynthesizer.h"
#include "MidiIO.h"
#include "GUI.h"
#include "ringbuffer.h"
#include "Event.h"
#include "Parameter.h"
#include "MidiMap.h"

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
      MidiMap* midiMap;
      MidiIO* midiIO;
      // jack_ringbuffer_t* midiEvents;

      void setInitialParameters();
};
