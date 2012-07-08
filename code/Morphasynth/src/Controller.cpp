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

#include "Controller.h"

Controller::Controller(): 
   gui(new GUI(this)),
   synth(new DSPSynthesizer()),
   audioIO(new AudioIO(synth)),
   midiMap(new MidiMap(this)),
   midiIO(new MidiIO(midiMap))
{
   setInitialParameters();
   audioIO->start();
   midiIO->start();
}

Controller::~Controller(){
   audioIO->stop();
   delete synth;
   delete gui;
   delete audioIO;
   delete midiIO;
   delete midiMap;
}

void Controller::addMidiEvent(MidiEvent e){
   synth->addMidiEvent(e);
}

/**
 * Receives an event from the GUI. Stores the new parameter value in the parameters vector and
 * forwards the event to the audio thread.
 */
void Controller::addGUIEvent(GuiEvent e){
   synth->addGUIEvent(e);
   parameters[e.parameter] = e.value;
}

void Controller::setInitialParameters(){
   // TODO: This should be loaded from a preset file.
   std::map<Parameter, float> initialParameters;
   initialParameters[NOISE_LEVEL] = 0.0;
   initialParameters[OSCILLATOR_FREQUENCY] = 330.0;
   initialParameters[OSCILLATOR_AMPLITUDE] = 0.8;
   initialParameters[OSCILLATOR_WAVEFORM] = 0.5;
   initialParameters[LOW_PASS_FREQUENCY] = 22000.0;
   initialParameters[LOW_PASS_RESONANCE] = 0.0;
   initialParameters[LOW_PASS_GAIN] = 1.0;
   initialParameters[LOW_PASS_KEYFOLLOW] = 0.0;
   initialParameters[HI_PASS_FREQUENCY] = 0.0;
   initialParameters[HI_PASS_RESONANCE] = 0.0;
   initialParameters[HI_PASS_GAIN] = 1.0;
   initialParameters[HI_PASS_KEYFOLLOW] = 0.0;
   initialParameters[ENVELOPE_ATTACK] = 0.001;
   initialParameters[ENVELOPE_DECAY] = 0.001;
   initialParameters[ENVELOPE_SUSTAIN] = 1.0;
   initialParameters[ENVELOPE_RELEASE] = 0.001;

   for (std::map<Parameter,float>::iterator i = initialParameters.begin(); i!=initialParameters.end(); ++i) {
      Parameter p = i->first;
      float v = i->second;

      // Keep the value in the parameters map for future use
      parameters[p] = v;

      // Send to synth thread
      GuiEvent e;
      e.parameter = p;
      e.value = v;
      synth->addGUIEvent(e);
      gui->addEvent(e);
   }
}
