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
   midiIO->stop();
   audioIO->stop();
   delete synth;
   delete gui;
   delete audioIO;
   delete midiIO;
   delete midiMap;
}

// void sendEventToGui(MidiEvent e){
// 
// }

void Controller::sendEventToSynth(MidiEvent e){
   synth->addMidiEvent(e);
}

void Controller::sendEventToGui(GuiEvent e){
   gui->addEvent(e);
}

void Controller::sendEventToSynth(GuiEvent e){
   synth->addGUIEvent(e);
}

void Controller::setInitialParameters(){
   // TODO: This should be loaded from a preset file.
   std::map<Parameter, float> initialParameters;

   initialParameters[NOISE_LEVEL] = 0.0;

   initialParameters[OSCILLATOR1_AMPLITUDE] = 0.5;
   initialParameters[OSCILLATOR1_WAVEFORM] = 0.5;
   initialParameters[OSCILLATOR1_FINETUNE] = 0.0;

   initialParameters[LFO_OSC1_WAVEFORM] = 0.0;
   initialParameters[LFO_OSC1_FREQUENCY] = 5.0;
   initialParameters[LFO_OSC1_TO_AMPLITUDE] = 0.0;
   initialParameters[LFO_OSC1_TO_FREQUENCY] = 0.0;

   initialParameters[OSCILLATOR2_AMPLITUDE] = 0.0;
   initialParameters[OSCILLATOR2_WAVEFORM] = 0.5;
   initialParameters[OSCILLATOR2_FINETUNE] = 0.0;

   initialParameters[LFO_OSC2_WAVEFORM] = 0.0;
   initialParameters[LFO_OSC2_FREQUENCY] = 5.0;
   initialParameters[LFO_OSC2_TO_AMPLITUDE] = 0.0;
   initialParameters[LFO_OSC2_TO_FREQUENCY] = 0.0;
   
   initialParameters[OSCILLATOR3_AMPLITUDE] = 0.0;
   initialParameters[OSCILLATOR3_WAVEFORM] = 0.5;
   initialParameters[OSCILLATOR3_FINETUNE] = 0.0;

   initialParameters[LFO_OSC3_WAVEFORM] = 0.0;
   initialParameters[LFO_OSC3_FREQUENCY] = 5.0;
   initialParameters[LFO_OSC3_TO_AMPLITUDE] = 0.0;
   initialParameters[LFO_OSC3_TO_FREQUENCY] = 0.0;

   initialParameters[HI_PASS_FREQUENCY] = 10.0;
   initialParameters[HI_PASS_RESONANCE] = 0.02;
   initialParameters[HI_PASS_KEYFOLLOW] = 1.0;
   initialParameters[HI_PASS_CONTOUR] = 0.0;

   initialParameters[HI_PASS_ATTACK] = 0.001;
   initialParameters[HI_PASS_DECAY] = 0.001;
   initialParameters[HI_PASS_SUSTAIN] = 1.0;
   initialParameters[HI_PASS_RELEASE] = 0.001;

   initialParameters[ENVELOPE_ATTACK] = 0.001;
   initialParameters[ENVELOPE_DECAY] = 0.001;
   initialParameters[ENVELOPE_SUSTAIN] = 1.0;
   initialParameters[ENVELOPE_RELEASE] = 0.001;

   initialParameters[LO_PASS_FREQUENCY] = 20000.0;
   initialParameters[LO_PASS_RESONANCE] = 0.75;
   initialParameters[LO_PASS_KEYFOLLOW] = 1.0;
   initialParameters[LO_PASS_CONTOUR] = 0.0;

   initialParameters[LO_PASS_ATTACK] = 0.001;
   initialParameters[LO_PASS_DECAY] = 0.001;
   initialParameters[LO_PASS_SUSTAIN] = 1.0;
   initialParameters[LO_PASS_RELEASE] = 0.001;

   initialParameters[ENVELOPE_ATTACK] = 0.001;
   initialParameters[ENVELOPE_DECAY] = 0.001;
   initialParameters[ENVELOPE_SUSTAIN] = 1.0;
   initialParameters[ENVELOPE_RELEASE] = 0.001;

   for (std::map<Parameter,float>::iterator i = initialParameters.begin(); i!=initialParameters.end(); ++i) {
      Parameter p = i->first;
      float v = i->second; // second es next 

      // Send to synth thread
      GuiEvent e;
      e.parameter = p;
      e.value = v; // es el del sinte y va potenciado pues cambio en el GUITimbre
      synth->addGUIEvent(e);
      gui->addEvent(e);
   }
}
