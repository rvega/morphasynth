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

#include "DSPSynthesizerVoice.h"

DSPSynthesizerVoice::DSPSynthesizerVoice():
   currentNote(0),
   silence(true),
   noise(new DSPNoiseWithLevel()),
   oscillator1(new DSPOscillator()),
   // lopass(new DSPLoPass()),
   // hipass(new DSPHiPass()),
   // currentNote(0),
   envelope(new ADSR()),
   output(0.0)
{ }

DSPSynthesizerVoice::~DSPSynthesizerVoice(){
   delete noise;
   delete oscillator1;
   // delete lopass;
   // delete hipass;
   delete envelope;
}

short int DSPSynthesizerVoice::getCurrentNote(){
   return currentNote;
}

void DSPSynthesizerVoice::setSampleRate(unsigned int sampleRate){
//    lopass->setSampleRate( sampleRate );
//    hipass->setSampleRate( sampleRate );
}


void DSPSynthesizerVoice::noteOn(short int noteNumber, short int velocity){
   silence = false;
   currentNote = noteNumber;

   float noteFrequency = MidiUtilities::getInstance().midiNote2Pitch(noteNumber);

   oscillator1->setFrequency(noteFrequency);
   envelope->keyOn();
}

void DSPSynthesizerVoice::noteOff(){
   envelope->keyOff();
}


void DSPSynthesizerVoice::setParameter(Parameter param, float value){
   switch(param){
      case(NOISE_LEVEL):
         noise->setLevel(value);
         break;

      case(OSCILLATOR1_AMPLITUDE):
         oscillator1->setAmplitude(value);
         break;
      case(OSCILLATOR1_WAVEFORM):
         oscillator1->setWaveform(value);
         break;

         /*
         // // // 
         // // // 
         // // //       case(LOW_PASS_FREQUENCY):
         // // //          lopass->setFrequency(value);
         // // //          break;
         // // //       case(LOW_PASS_RESONANCE):
         // // //          lopass->setResonance(value);
         // // //          break;
         // // //       case(LOW_PASS_GAIN):
         // // //          lopass->setGain(value);
         // // //          break;
         // // //       case(LOW_PASS_KEYFOLLOW):
         // // //          // TODO
         // // //          break;
         // // // 
         // // // 
         // // // 
         // // //       case(HI_PASS_FREQUENCY):
         // // //          hipass->setFrequency(value);
         // // //          break;
         // // //       case(HI_PASS_RESONANCE):
         // // //          hipass->setResonance(value);
         // // //          break;
         // // //       case(HI_PASS_GAIN):
         // // //          hipass->setGain(value);
         // // //          break;
         // // //       case(HI_PASS_KEYFOLLOW):
         // // //          // TODO
         // // //          break;
         // // // 
         // // // 
         // // // 
         */ 
      case(ENVELOPE_ATTACK):
         envelope->setAttackTime(value);
         break;
      case(ENVELOPE_DECAY):
         envelope->setDecayTime(value);
         break;
      case(ENVELOPE_SUSTAIN):
         envelope->setSustainLevel(value);
         break;
      case(ENVELOPE_RELEASE):
         envelope->setReleaseTime(value);
         break;

      default:
         std::cout << "Synth is trying to set unknown parameter from received event " << param << ": " << value << "\n";
         break;
   }
}

void DSPSynthesizerVoice::process(StkFloat* out, unsigned int bufferSize){
   // Silence
   if(envelope->getState() == ADSR::IDLE) silence = true;
   if(silence){
      for(int i=0; i<bufferSize; i++){
         *out++ = 0.0;
         *out++ = 0.0;
      }
      return;
   }

   // Sound
   for(int i=0; i<bufferSize; i++){
      output = (oscillator1->tick() + noise->tick()) * envelope->tick();
      *out++ = output;
      *out++ = output;
   }
}
