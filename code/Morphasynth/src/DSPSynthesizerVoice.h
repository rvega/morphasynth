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

#include <iostream>
#include <string.h>

#include "MidiUtilities.h"
#include "Parameter.h"
#include "DSPOscillator.h"
#include "ADSR.h"
#include "DSPNoiseWithLevel.h"
#include "DSPLoPass.h"
#include "DSPHiPass.h"

#define checkMax(x,y) (((x)>=(y)) ? (x) : (y))
#define checkMin(x,y) (((x)<(y)) ? (x) : (y))
#define checkRange(value, min, max) checkMax(min, checkMin(max, value))

using namespace stk;

class DSPSynthesizerVoice{
   private:
      // State
      short int currentNote;
      StkFloat currentNoteFrequency;
      bool silence;

      // Internal signals:
      StkFloat output;
      StkFloat oscillators;
      StkFloat lfo1Out;
      StkFloat lfo2Out;
      StkFloat lfo3Out;
      StkFloat freq;
      StkFloat amp;
      
      // Parameters
      StkFloat noiseLevel;

      StkFloat oscillator1Amplitude;
      StkFloat oscillator1Waveform;
      StkFloat oscillator1Finetune;

      StkFloat lfo1Waveform;
      StkFloat lfo1Frequency;
      StkFloat lfo1ToAmplitude;
      StkFloat lfo1ToFrequency;

      StkFloat oscillator2Amplitude;
      StkFloat oscillator2Waveform;
      StkFloat oscillator2Finetune;

      StkFloat lfo2Waveform;
      StkFloat lfo2Frequency;
      StkFloat lfo2ToAmplitude;
      StkFloat lfo2ToFrequency;

      StkFloat oscillator3Amplitude;
      StkFloat oscillator3Waveform;
      StkFloat oscillator3Finetune;

      StkFloat lfo3Waveform;
      StkFloat lfo3Frequency;
      StkFloat lfo3ToAmplitude;
      StkFloat lfo3ToFrequency;

      StkFloat hiPassFrequency;
      StkFloat hiPassResonance;
      StkFloat hiPassKeyFollow;
      StkFloat hiPassContour;

      StkFloat hiPassAttack;
      StkFloat hiPassDecay;
      StkFloat hiPassSustain;
      StkFloat hiPassRelease;

      StkFloat lfoHiPassWaveform;
      StkFloat lfoHiPassFrequency;
      StkFloat lfoHiPassAmplitude;

      StkFloat loPassFrequency;
      StkFloat loPassResonance;
      StkFloat loPassKeyFollow;
      StkFloat loPassContour;

      StkFloat loPassAttack;
      StkFloat loPassDecay;
      StkFloat loPassSustain;
      StkFloat loPassRelease;

      StkFloat lfoLoPassWaveform;
      StkFloat lfoLoPassFrequency;
      StkFloat lfoLoPassAmplitude;

      StkFloat envelopeAttack;
      StkFloat envelopeDecay;
      StkFloat envelopeSustain;
      StkFloat envelopeRelease;

      // DSP Blocks:
      DSPNoiseWithLevel* noise;
      DSPOscillator* oscillator1;
      DSPOscillator* lfo1;
      DSPOscillator* oscillator2;
      DSPOscillator* lfo2;
      DSPOscillator* oscillator3;
      DSPOscillator* lfo3;
      DSPHiPass* hiPass;
      ADSR* hiPassEnvelope;
      DSPOscillator* lfoHiPass;
      DSPLoPass* loPass;
      ADSR* loPassEnvelope;
      DSPOscillator* lfoLoPass;
      ADSR* envelope;

   public:
      DSPSynthesizerVoice();
      ~DSPSynthesizerVoice();

      bool isSilent();
      void setSampleRate(unsigned int sampleRate);
      short int getCurrentNote();
      void setParameter(Parameter param, float value);
      void noteOn(short int noteNumber, short int velocity);
      void noteOff();
      inline StkFloat tick();
};


//============================//
//  DSP BLOCKS & CONNECTIONS  //
//============================//
inline StkFloat DSPSynthesizerVoice::tick(){
   // Silence?
   if(envelope->getState() == ADSR::IDLE) {
      silence = true;
      return 0.0;
   }
   // if(silence){
   //    for(int i=0; i<bufferSize; i++){
   //       *out++ = 0.0;
   //       *out++ = 0.0;
   //    }
   //    return;
   // }

   // Sound
   // for(int i=0; i<bufferSize; i++){
      // Oscillator 1
      lfo1Out = lfo1->tick();
      freq = currentNoteFrequency * pow( 2, (oscillator1Finetune/12.0)+(lfo1ToFrequency * lfo1Out) );
      amp = oscillator1Amplitude * (1 + lfo1ToAmplitude*lfo1Out);
      oscillator1->setFrequency(freq);
      oscillator1->setAmplitude(amp);

      // Oscillator 2
      lfo2Out = lfo2->tick();
      freq = currentNoteFrequency * pow( 2, (oscillator2Finetune/12.0)+(lfo2ToFrequency * lfo2Out) );
      amp = oscillator2Amplitude * (1 + lfo2ToAmplitude*lfo2Out);
      oscillator2->setFrequency(freq);
      oscillator2->setAmplitude(amp);

      // Oscillator 3
      lfo3Out = lfo3->tick();
      freq = currentNoteFrequency * pow( 2, (oscillator3Finetune/12.0)+(lfo3ToFrequency * lfo3Out) );
      freq = freq / 2.0;
      amp = oscillator3Amplitude * (1 + lfo3ToAmplitude*lfo3Out);
      oscillator3->setFrequency(freq);
      oscillator3->setAmplitude(amp);

      oscillators = oscillator1->tick() + oscillator2->tick() + oscillator3->tick() + noise->tick() ;

      //Hi Pass
      freq = (hiPassFrequency + (currentNoteFrequency * hiPassKeyFollow)) \
             * pow (2, (hiPassEnvelope->tick()*hiPassContour) + ((1.0+lfoHiPass->tick()) * lfoHiPassAmplitude)); 
      hiPass->setFrequency( checkMin(20000.0, freq) );

      // Lo Pass
      freq = (loPassFrequency + (currentNoteFrequency * loPassKeyFollow)) \
             * pow (2, (loPassEnvelope->tick()*loPassContour) + ((1.0+lfoLoPass->tick()) * lfoLoPassAmplitude)); 
      loPass->setFrequency( checkMin(20000.0, freq) );

      // All together, now!
      output = loPass->tick( hiPass->tick(oscillators) ) * envelope->tick();

      return output;
      // *out++ = output;
      // *out++ = output;
   // }
}
