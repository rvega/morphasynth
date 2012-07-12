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

#define checkMax(x,y) (((x)>(y)) ? (x) : (y))
#define checkMin(x,y) (((x)<(y)) ? (x) : (y))
#define checkRange(value, min, max) checkMax(min, checkMin(max, value))

//======================//
//  INIT & DESTRUCTION  //
//======================//
DSPSynthesizerVoice::DSPSynthesizerVoice():
   currentNote(0),
   currentNoteFrequency(0.0),
   silence(true),

   output(0.0),

   oscillator1Amplitude(0.0),
   oscillator1Waveform(0.0),
   oscillator1Finetune(0.0),

   lfo1Waveform(0.0),
   lfo1Frequency(0.0),
   lfo1ToAmplitude(0.0),
   lfo1ToFrequency(0.0),
   
   envelopeAttack(0.0),
   envelopeDecay(0.0),
   envelopeSustain(0.0),
   envelopeRelease(0.0),

   noise(new DSPNoiseWithLevel()),
   oscillator1(new DSPOscillator()),
   lfo1(new DSPOscillator()),
   // lopass(new DSPLoPass()),
   // hipass(new DSPHiPass()),
   // currentNote(0),
   envelope(new ADSR())
{ 
   lfo1->setAmplitude(1.0);
}

DSPSynthesizerVoice::~DSPSynthesizerVoice(){
   delete noise;
   delete oscillator1;
   delete lfo1;
   // delete lopass;
   // delete hipass;
   delete envelope;
}

//=====================//
//  GETTERS & SETTERS  //
//=====================//
short int DSPSynthesizerVoice::getCurrentNote(){
   return currentNote;
}

void DSPSynthesizerVoice::setSampleRate(unsigned int sampleRate){
//    lopass->setSampleRate( sampleRate );
//    hipass->setSampleRate( sampleRate );
}

void DSPSynthesizerVoice::setParameter(Parameter param, float value){
   StkFloat f;

   switch(param){
      case(NOISE_LEVEL):
         noiseLevel = checkRange(value, 0.0, 1.0);
         noise->setLevel(noiseLevel);
         break;

      case(OSCILLATOR1_AMPLITUDE):
         oscillator1Amplitude = checkRange(value, 0.0, 1.0);
         oscillator1->setAmplitude(oscillator1Amplitude);
         break;
      case(OSCILLATOR1_WAVEFORM):
         oscillator1Waveform = checkRange(value, 0.0, 1.0);
         oscillator1->setWaveform(oscillator1Waveform);
         break;
      case(OSCILLATOR1_FINETUNE):
         oscillator1Finetune = checkRange(value, -1.0, 1.0);
         oscillator1->setFrequency(f);
         break;

      case(LFO_OSC1_WAVEFORM):
         lfo1Waveform = checkRange(value, 0.0, 1.0);
         lfo1->setWaveform(lfo1Waveform);
         break;
      case(LFO_OSC1_FREQUENCY):
         lfo1Frequency = checkRange(value, 0.0, 200.0);
         lfo1->setFrequency(lfo1Frequency);
         break;
      case(LFO_OSC1_TO_AMPLITUDE):
         lfo1ToAmplitude = checkRange(value, 0.0, 1.0);
         break;
      case(LFO_OSC1_TO_FREQUENCY):
         lfo1ToFrequency = checkRange(value, 0.0, 3.0);
         break;

      case(ENVELOPE_ATTACK):
         envelopeAttack = checkRange(value, 0.001, 10.0);
         envelope->setAttackTime(envelopeAttack);
         break;
      case(ENVELOPE_DECAY):
         envelopeDecay = checkRange(value, 0.001, 10.0);
         envelope->setDecayTime(envelopeDecay);
         break;
      case(ENVELOPE_SUSTAIN):
         envelopeSustain = checkRange(value, 0.0, 1.0);
         envelope->setSustainLevel(envelopeSustain);
         break;
      case(ENVELOPE_RELEASE):
         envelopeRelease = checkRange(value, 0.001, 10.0);
         envelope->setReleaseTime(envelopeRelease);
         break;

      default:
         std::cout << "Synth is trying to set unknown parameter from received event " << param << ": " << value << "\n";
         break;
   }
}

StkFloat DSPSynthesizerVoice::calculateFrequency(short int midiNote, StkFloat finetune){
   StkFloat midiFreq = MidiUtilities::getInstance().midiNote2Pitch(midiNote);
   return midiFreq*pow(2.0, finetune/12.0);
}

//======================//
//  NOTE ON & NOTE OFF  //
//======================//
void DSPSynthesizerVoice::noteOn(short int noteNumber, short int velocity){
   silence = false;
   currentNote = noteNumber;
   currentNoteFrequency = MidiUtilities::getInstance().midiNote2Pitch(noteNumber);

   oscillator1->reset();
   lfo1->reset();
   envelope->keyOn();
}

void DSPSynthesizerVoice::noteOff(){
   envelope->keyOff();
}

//============================//
//  DSP BLOCKS & CONNECTIONS  //
//============================//
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
      freq = currentNoteFrequency * pow( 2, (oscillator1Finetune/12.0)+(lfo1ToFrequency * lfo1->tick()) );
      oscillator1->setFrequency(freq);

      output = (oscillator1->tick() + noise->tick()) * envelope->tick();
      *out++ = output;
      *out++ = output;
   }
}
