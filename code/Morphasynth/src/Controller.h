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

#include "AudioIO.h"
#include "DSPSynthesizer.h"
#include "MidiIO.h"
#include "GUI.h"
#include "ringbuffer.h"
#include "GuiEvent.h"
#include "Parameter.h"
#include "MidiMap.h"

class Controller{
	public:
      Controller();
      ~Controller();

		void addMidiEvent(MidiEvent e);
      void addGUIEvent(GuiEvent e);

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
