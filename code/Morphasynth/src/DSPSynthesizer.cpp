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
      for (int i=0; i<10; i++) {
         DSPSynthesizerVoice* aVoice = new DSPSynthesizerVoice();
         voices->push_back(aVoice);
      }
}

DSPSynthesizer::~DSPSynthesizer(){
   jack_ringbuffer_free(guiEventsBuffer);
   jack_ringbuffer_free(midiEventsBuffer);

   for(int i=0; i<10; ++i){
      delete voices->at(i);
   }
   delete voices;
}

void DSPSynthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate ); // Sets the sample rate for all STK objects
   
   for(int i=0; i<10; ++i){
      voices->at(i)->setSampleRate(sampleRate);
   }
}

void DSPSynthesizer::processGuiEvent(GuiEvent* event){
   for(int i = 0; i<10; i++) {
      voices->at(i)->setParameter(event->parameter, event->value);
   }
}

inline void DSPSynthesizer::processMidiEvent(MidiEvent* event){
   switch(event->command){
      case(NOTE_ON):
         // Look for an available voice
         for(int i=0; i<10; ++i){
            voice = voices->at(i);
            if(voice->isSilent()){
               // std::cout << "Note on voice " << i << " " << event->value1 << "\n";
               voice->noteOn(event->value1, event->value2);
               break;
            }
         }
         break;

      case(NOTE_OFF):
         for(int i=0; i<10; ++i){
            voice = voices->at(i);
            // std::cout << "Note off voice " << i << " " << event->value1 << " " << voice->getCurrentNote() << "\n";
            if(!voice->isSilent() && voice->getCurrentNote() == event->value1){
               voice->noteOff();
               // std::cout << "Y" << "\n";
               // break;
            }
         }
         // std::cout << "off break 2" << "\n";
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
   // Process events from GUI
   while(jack_ringbuffer_read_space(guiEventsBuffer) >= sizeof(GuiEvent)){ 
      jack_ringbuffer_read(guiEventsBuffer, (char*)&guiEvent, sizeof(GuiEvent));
      processGuiEvent(&guiEvent);
   }

   // Make some sound
   out = (StkFloat*) outBuffer;
   for(int i=0; i<bufferSize; i++){

      // Process events from MIDI
      while(jack_ringbuffer_read_space(midiEventsBuffer) >= sizeof(MidiEvent)){ // If there's at least one event to read
         jack_ringbuffer_read(midiEventsBuffer, (char*)&midiEvent, sizeof(MidiEvent));
         processMidiEvent(&midiEvent);
      }

      voiceOut = 0;
      for(int j=0; j<10; ++j){
         voice = voices->at(j);
         if(!voice->isSilent()){
            voiceOut += voice->tick(); 
         }
         // voiceOut = voiceOut/10.0;
      }
      // std::cout << voiceOut << "\n";
      *out++ = voiceOut;
      *out++ = voiceOut;
   }
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
