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
   sampleRate(0), 
   outputBuffer(NULL), 
   midiUtils(new MidiUtilities()),
   noise(new DSPNoiseWithLevel()),
   oscillator1(new DSPOscillator()),
   envelope(new ADSR()),
   lopass(new DSPLoPass()),
   hipass(new DSPHiPass()),
   currentNote(0),
   out(0.0), 
   s1(0.0)
{ 
      midiEventsBuffer = jack_ringbuffer_create(128 * sizeof(MidiEvent));
      jack_ringbuffer_mlock(midiEventsBuffer);

      guiEventsBuffer = jack_ringbuffer_create(128 * sizeof(GuiEvent));
      jack_ringbuffer_mlock(guiEventsBuffer);

      midiUtils->initMidiNotes();
}

DSPSynthesizer::~DSPSynthesizer(){
   jack_ringbuffer_free(guiEventsBuffer);
   jack_ringbuffer_free(midiEventsBuffer);
   delete midiUtils;
   delete noise;
   delete oscillator1;
   delete envelope;
   delete lopass;
   delete hipass;
}

void DSPSynthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate );
   lopass->setSampleRate( sampleRate );
   hipass->setSampleRate( sampleRate );
}

void DSPSynthesizer::processGuiEvent(GuiEvent* event){
   switch(event->parameter){

      case(NOISE_LEVEL):
         noise->setLevel(event->value);
         break;
      case(OSCILLATOR_FREQUENCY):
         oscillator1->setFrequency(event->value);
         break;
      case(OSCILLATOR_AMPLITUDE):
         oscillator1->setAmplitude(event->value);
         break;
      case(OSCILLATOR_WAVEFORM):
         oscillator1->setWaveform(event->value);
         break;


      case(LOW_PASS_FREQUENCY):
         lopass->setFrequency(event->value);
         break;
      case(LOW_PASS_RESONANCE):
         lopass->setResonance(event->value);
         break;
      case(LOW_PASS_GAIN):
         lopass->setGain(event->value);
         break;
      case(LOW_PASS_KEYFOLLOW):
         // TODO
         break;



      case(HI_PASS_FREQUENCY):
         hipass->setFrequency(event->value);
         break;
      case(HI_PASS_RESONANCE):
         hipass->setResonance(event->value);
         break;
      case(HI_PASS_GAIN):
         hipass->setGain(event->value);
         break;
      case(HI_PASS_KEYFOLLOW):
         // TODO
         break;



      case(ENVELOPE_ATTACK):
         envelope->setAttackTime(event->value);
         break;
      case(ENVELOPE_DECAY):
         envelope->setDecayTime(event->value);
         break;
      case(ENVELOPE_SUSTAIN):
         envelope->setSustainLevel(event->value);
         break;
      case(ENVELOPE_RELEASE):
         envelope->setReleaseTime(event->value);
         break;


      default:
         std::cout << "Synth is trying to set unknown parameter from received event " << event->parameter << ": " << event->value << "\n";
         break;
   }
}

void DSPSynthesizer::processMidiEvent(MidiEvent* event){
   switch(event->command){
      case(NOTE_ON):
         oscillator1->setFrequency(midiUtils->midiNote2Pitch(event->value1));
         oscillator1->reset();
         envelope->keyOn();
         currentNote = event->value1;
         break;
      case(NOTE_OFF):
            // If the note off even is not for the note we're playing, ignore it.
            if(event->value1 == currentNote){
               envelope->keyOff();
            }
         break;
      case(PITCH_BEND):
         // TODO
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
   outputBuffer = (StkFloat *)outBuffer;   
   for(unsigned int i = 0; i < bufferSize; i++) {
      s1 = noise->tick() + oscillator1->tick();
      out = lopass->tick( hipass->tick(s1) ) * envelope->tick();
      *outputBuffer++ = out;
      *outputBuffer++ = out;
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
