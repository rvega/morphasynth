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

#include "DSPSynthesizer.h"

DSPSynthesizer::DSPSynthesizer(): 
   sampleRate(0)
{ 
      midiEventsBuffer = jack_ringbuffer_create(128 * sizeof(MidiEvent));
      jack_ringbuffer_mlock(midiEventsBuffer);

      guiEventsBuffer = jack_ringbuffer_create(128 * sizeof(GuiEvent));
      jack_ringbuffer_mlock(guiEventsBuffer);
   
      voices = new std::vector<DSPSynthesizerVoice*>;
      DSPSynthesizerVoice* aVoice = new DSPSynthesizerVoice(); // Just one voice for now
      voices->push_back(aVoice);
}

DSPSynthesizer::~DSPSynthesizer(){
   jack_ringbuffer_free(guiEventsBuffer);
   jack_ringbuffer_free(midiEventsBuffer);

   for(int i=0; i<voices->size(); ++i){
      delete voices->at(i);
   }
   delete voices;
}

void DSPSynthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate ); // Sets the sample rate for all STK objects
   
   for(int i=0; i<voices->size(); ++i){
      voices->at(i)->setSampleRate(sampleRate);
   }
}

void DSPSynthesizer::processGuiEvent(GuiEvent* event){
   for(int i = 0; i<voices->size(); i++) {
      voices->at(i)->setParameter(event->parameter, event->value);
   }
}

void DSPSynthesizer::processMidiEvent(MidiEvent* event){
   switch(event->command){
      case(NOTE_ON):
         voices->at(0)->noteOn(event->value1, event->value2);
         break;

      case(NOTE_OFF):
         if(voices->at(0)->getCurrentNote() == event->value1){
            voices->at(0)->noteOff();
         }
         break;

      case(PITCH_BEND):
         // TODO: Pitchbend
         break;

      default:
         std::cout << "Synth received an unknown MIDI message: " << event->command << "\n";
         break;
   }
}
int DSPSynthesizer::process(void *outBuffer, void *inBuffer, unsigned int bufferSize){
   // Process events from MIDI
   while(jack_ringbuffer_read_space(midiEventsBuffer) >= sizeof(MidiEvent)){ // If there's at least one event to read
      jack_ringbuffer_read(midiEventsBuffer, (char*)&midiEvent, sizeof(MidiEvent));
      processMidiEvent(&midiEvent);
   }

   // Process events from GUI
   while(jack_ringbuffer_read_space(guiEventsBuffer) >= sizeof(GuiEvent)){ 
      jack_ringbuffer_read(guiEventsBuffer, (char*)&guiEvent, sizeof(GuiEvent));
      processGuiEvent(&guiEvent);
   }

   // Make some sound
   voices->at(0)->process((StkFloat*) outBuffer, bufferSize); // Only one voice for now
   return 0;
}

void DSPSynthesizer::addMidiEvent(MidiEvent event){
   if(jack_ringbuffer_write_space(midiEventsBuffer) < sizeof(MidiEvent)){
      std::cout << "Could not add midi event" << event.command << ": " << event.value1 << " to synth's event buffer." << "\n";
   }
   else{
      jack_ringbuffer_write(midiEventsBuffer, (const char *)&event, sizeof(MidiEvent));
   }
}

void DSPSynthesizer::addGUIEvent(GuiEvent event){
   if(jack_ringbuffer_write_space(guiEventsBuffer) < sizeof(GuiEvent)){
      std::cout << "Could not add gui event" << event.parameter << ": " << event.value << " to synth's event buffer." << "\n";
   }
   else{
      jack_ringbuffer_write(guiEventsBuffer, (const char *)&event, sizeof(GuiEvent));
   }
}
