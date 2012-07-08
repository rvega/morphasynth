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

#include "Oscillator.h"
#include <math.h>

Oscillator::Oscillator(): 
   amplitude(0.0), 
   frequency(440.0), 
   waveform(0.5),
   sine(new SineWave()),
   square(new BlitSquare()),
   saw(new BlitSaw()),
   out(0.0)      
{
      //Constructor
      sine->setFrequency(220.0);
}

Oscillator::~Oscillator(){
   delete sine;
   delete square;
   delete saw;
}

void Oscillator::setAmplitude(StkFloat amp){
   if(amp > 1.0) amp = 1.0;
   else if(amp < 0.0) amp = 0.0;
   amplitude = amp;
}

void Oscillator::setFrequency(StkFloat freq){
   if(freq > 4200.0) freq=4200.0;
   else if(freq < 27) freq=27.0;
   frequency = freq;

   sine->setFrequency(frequency);

   square->setFrequency(frequency);
   // unsigned int numHarmonics = floor(Stk::sampleRate()/(4*frequency) - 0.5);
   // square->setHarmonics(numHarmonics);

   saw->setFrequency(frequency);
   // numHarmonics = floor(Stk::sampleRate()/(2*frequency) - 1 );
   // std::cout << numHarmonics << "\n";
   // saw->setHarmonics(numHarmonics);
}


void Oscillator::setWaveform(StkFloat wf){
   if(wf > 1.0) wf = 1.0;
   else if(wf < 0.0) wf = 0.0;
   waveform = wf;
}

void Oscillator::reset(){
   sine->reset();
   square->reset();
   saw->reset();
   // sine->setPhase(0.0);
   // square->setPhase(0.0);
   // saw->setPhase(0.0);
}
