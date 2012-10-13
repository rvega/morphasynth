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
   std::vector<GuiEvent> parameters = PresetManager::getParametersForPreset("1. SIMPLE SINE");
   for(std::vector<GuiEvent>::size_type i = 0; i < parameters.size(); i++){
      GuiEvent event = parameters[i];
      sendEventToSynth(event);
      sendEventToGui(event);
   }
}
