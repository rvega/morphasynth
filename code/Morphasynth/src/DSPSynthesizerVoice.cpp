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

#define checkMax(x,y) (((x)>=(y)) ? (x) : (y))
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
   oscillators(0.0),

   oscillator1Amplitude(0.0),
   oscillator1Waveform(0.0),
   oscillator1Finetune(0.0),

   lfo1Waveform(0.0),
   lfo1Frequency(0.0),
   lfo1ToAmplitude(0.0),
   lfo1ToFrequency(0.0),
   
   oscillator2Amplitude(0.0),
   oscillator2Waveform(0.0),
   oscillator2Finetune(0.0),

   lfo2Waveform(0.0),
   lfo2Frequency(0.0),
   lfo2ToAmplitude(0.0),
   lfo2ToFrequency(0.0),
   
   oscillator3Amplitude(0.0),
   oscillator3Waveform(0.0),
   oscillator3Finetune(0.0),

   lfo3Waveform(0.0),
   lfo3Frequency(0.0),
   lfo3ToAmplitude(0.0),
   lfo3ToFrequency(0.0),
   
   hiPassFrequency(0.0),
   hiPassResonance(0.0),
   hiPassKeyFollow(0.0),
   hiPassContour(0.0),
   
   hiPassAttack(0.0),
   hiPassDecay(0.0),
   hiPassSustain(0.0),
   hiPassRelease(0.0),
   
   lfoHiPassWaveform(0.0),
   lfoHiPassFrequency(0.0),
   lfoHiPassAmplitude(0.0),

   loPassFrequency(0.0),
   loPassResonance(0.0),
   loPassKeyFollow(0.0),
   loPassContour(0.0),
   
   loPassAttack(0.0),
   loPassDecay(0.0),
   loPassSustain(0.0),
   loPassRelease(0.0),
   
   lfoLoPassWaveform(0.0),
   lfoLoPassFrequency(0.0),
   lfoLoPassAmplitude(0.0),

   envelopeAttack(0.0),
   envelopeDecay(0.0),
   envelopeSustain(0.0),
   envelopeRelease(0.0),

   noise(new DSPNoiseWithLevel()),
   oscillator1(new DSPOscillator()),
   lfo1(new DSPOscillator()),
   oscillator2(new DSPOscillator()),
   lfo2(new DSPOscillator()),
   oscillator3(new DSPOscillator()),
   lfo3(new DSPOscillator()),
   hiPass(new DSPHiPass()),
   hiPassEnvelope(new ADSR()),
   lfoHiPass(new DSPOscillator()),
   loPass(new DSPLoPass()),
   loPassEnvelope(new ADSR()),
   lfoLoPass(new DSPOscillator()),
   envelope(new ADSR())
{ 
   lfo1->setAmplitude(1.0);
   lfo2->setAmplitude(1.0);
   lfo3->setAmplitude(1.0);
   lfoHiPass->setAmplitude(0.5);
   lfoLoPass->setAmplitude(0.5);
}

DSPSynthesizerVoice::~DSPSynthesizerVoice(){
   delete noise;
   delete oscillator1;
   delete lfo1;
   delete oscillator2;
   delete lfo2;
   delete oscillator3;
   delete lfo3;
   delete hiPass;
   delete hiPassEnvelope;
   delete lfoHiPass;
   delete loPass;
   delete loPassEnvelope;
   delete lfoLoPass;
   delete envelope;
}

//=====================//
//  GETTERS & SETTERS  //
//=====================//
short int DSPSynthesizerVoice::getCurrentNote(){
   return currentNote;
}

void DSPSynthesizerVoice::setSampleRate(unsigned int sampleRate){
   loPass->setSampleRate( sampleRate );
   hiPass->setSampleRate( sampleRate );
}

void DSPSynthesizerVoice::setParameter(Parameter param, float value){
   switch(param){
      case(NOISE_LEVEL):
         noiseLevel = checkRange(value, 0.0, 1.0);
         noise->setLevel(noiseLevel);
         break;

      case(OSCILLATOR1_AMPLITUDE):
         oscillator1Amplitude = checkRange(value, 0.0, 1.0);
         break;
      case(OSCILLATOR1_WAVEFORM):
         oscillator1Waveform = checkRange(value, 0.0, 1.0);
         oscillator1->setWaveform(oscillator1Waveform);
         break;
      case(OSCILLATOR1_FINETUNE):
         oscillator1Finetune = checkRange(value, -1.0, 1.0);
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

      case(OSCILLATOR2_AMPLITUDE):
         oscillator2Amplitude = checkRange(value, 0.0, 1.0);
         break;
      case(OSCILLATOR2_WAVEFORM):
         oscillator2Waveform = checkRange(value, 0.0, 1.0);
         oscillator2->setWaveform(oscillator2Waveform);
         break;
      case(OSCILLATOR2_FINETUNE):
         oscillator2Finetune = checkRange(value, -1.0, 1.0);
         break;

      case(LFO_OSC2_WAVEFORM):
         lfo2Waveform = checkRange(value, 0.0, 1.0);
         lfo2->setWaveform(lfo2Waveform);
         break;
      case(LFO_OSC2_FREQUENCY):
         lfo2Frequency = checkRange(value, 0.0, 200.0);
         lfo2->setFrequency(lfo2Frequency);
         break;
      case(LFO_OSC2_TO_AMPLITUDE):
         lfo2ToAmplitude = checkRange(value, 0.0, 1.0);
         break;
      case(LFO_OSC2_TO_FREQUENCY):
         lfo2ToFrequency = checkRange(value, 0.0, 3.0);
         break;
         
      case(OSCILLATOR3_AMPLITUDE):
         oscillator3Amplitude = checkRange(value, 0.0, 1.0);
         break;
      case(OSCILLATOR3_WAVEFORM):
         oscillator3Waveform = checkRange(value, 0.0, 1.0);
         oscillator3->setWaveform(oscillator3Waveform);
         break;
      case(OSCILLATOR3_FINETUNE):
         oscillator3Finetune = checkRange(value, -1.0, 1.0);
         break;

      case(LFO_OSC3_WAVEFORM):
         lfo3Waveform = checkRange(value, 0.0, 1.0);
         lfo3->setWaveform(lfo3Waveform);
         break;
      case(LFO_OSC3_FREQUENCY):
         lfo3Frequency = checkRange(value, 0.0, 200.0);
         lfo3->setFrequency(lfo3Frequency);
         break;
      case(LFO_OSC3_TO_AMPLITUDE):
         lfo3ToAmplitude = checkRange(value, 0.0, 1.0);
         break;
      case(LFO_OSC3_TO_FREQUENCY):
         lfo3ToFrequency = checkRange(value, 0.0, 3.0);
         break;

      case(HI_PASS_FREQUENCY):
         hiPassFrequency = checkRange(value, 10.0, 20000.0);
         break;
      case(HI_PASS_RESONANCE):
         hiPassResonance = checkRange(value, 0.02, 1.0);
         hiPass->setResonance(hiPassResonance);
         break;
      case(HI_PASS_KEYFOLLOW):
         hiPassKeyFollow = checkRange(value, 0.0, 1.0);
         break;
      case(HI_PASS_CONTOUR):
         hiPassContour = checkRange(value, -100.0, 100.0);
         break;

      case(HI_PASS_ATTACK):
         hiPassAttack = checkRange(value, 0.001, 10.0);
         hiPassEnvelope->setAttackTime(hiPassAttack);
         break;
      case(HI_PASS_DECAY):
         hiPassDecay = checkRange(value, 0.001, 10.0);
         hiPassEnvelope->setDecayTime(hiPassDecay);
         break;
      case(HI_PASS_SUSTAIN):
         hiPassSustain = checkRange(value, 0.0, 1.0);
         hiPassEnvelope->setSustainLevel(hiPassSustain);
         break;
      case(HI_PASS_RELEASE):
         hiPassRelease = checkRange(value, 0.001, 10.0);
         hiPassEnvelope->setReleaseTime(hiPassRelease);
         break;
         
      case(LFO_HI_PASS_WAVEFORM):
         lfoHiPassWaveform = checkRange(value, 0.0, 1.0);
         lfoHiPass->setWaveform(lfoHiPassWaveform);
         break;
      case(LFO_HI_PASS_FREQUENCY):
         lfoHiPassFrequency = checkRange(value, 0.0, 200.0);
         lfoHiPass->setFrequency(lfoHiPassFrequency);
         break;
      case(LFO_HI_PASS_AMPLITUDE):
         lfoHiPassAmplitude = checkRange(value, 0.0, 5.0);
         break;

      case(LO_PASS_FREQUENCY):
         loPassFrequency = checkRange(value, 10.0, 20000.0);
         break;
      case(LO_PASS_RESONANCE):
         loPassResonance = checkRange(value, 0.0, 1.0);
         loPass->setResonance(loPassResonance);
         break;
      case(LO_PASS_KEYFOLLOW):
         loPassKeyFollow = checkRange(value, 0.0, 1.0);
         break;
      case(LO_PASS_CONTOUR):
         loPassContour = checkRange(value, -100.0, 100.0);
         break;

      case(LO_PASS_ATTACK):
         loPassAttack = checkRange(value, 0.001, 10.0);
         loPassEnvelope->setAttackTime(loPassAttack);
         break;
      case(LO_PASS_DECAY):
         loPassDecay = checkRange(value, 0.001, 10.0);
         loPassEnvelope->setDecayTime(loPassDecay);
         break;
      case(LO_PASS_SUSTAIN):
         loPassSustain = checkRange(value, 0.0, 1.0);
         loPassEnvelope->setSustainLevel(loPassSustain);
         break;
      case(LO_PASS_RELEASE):
         loPassRelease = checkRange(value, 0.001, 10.0);
         loPassEnvelope->setReleaseTime(loPassRelease);
         break;
         
      case(LFO_LO_PASS_WAVEFORM):
         lfoLoPassWaveform = checkRange(value, 0.0, 1.0);
         lfoLoPass->setWaveform(lfoLoPassWaveform);
         break;
      case(LFO_LO_PASS_FREQUENCY):
         lfoLoPassFrequency = checkRange(value, 0.0, 200.0);
         lfoLoPass->setFrequency(lfoLoPassFrequency);
         break;
      case(LFO_LO_PASS_AMPLITUDE):
         lfoLoPassAmplitude = checkRange(value, 0.0, 5.0);
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

//======================//
//  NOTE ON & NOTE OFF  //
//======================//
void DSPSynthesizerVoice::noteOn(short int noteNumber, short int velocity){
   silence = false;
   currentNote = noteNumber;
   currentNoteFrequency = MidiUtilities::getInstance().midiNote2Pitch(noteNumber);

   oscillator1->reset();
   lfo1->reset();
   oscillator2->reset();
   lfo2->reset();
   oscillator3->reset();
   lfo3->reset();

   hiPassEnvelope->keyOn();
   loPassEnvelope->keyOn();
   envelope->keyOn();
}

void DSPSynthesizerVoice::noteOff(){
   hiPassEnvelope->keyOff();
   loPassEnvelope->keyOff();
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

      *out++ = output;
      *out++ = output;
   }
}
