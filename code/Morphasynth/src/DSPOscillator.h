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

#include "Stk.h"
#include "SineWave.h"
#include "BlitSquare.h"
#include "BlitSaw.h"

using namespace stk;

class DSPOscillator{
   protected:
      StkFloat amplitude;
      StkFloat frequency;
      StkFloat waveform;

      SineWave* sine;
      BlitSquare* square;
      BlitSaw* saw;

      StkFloat out;

   public:
      DSPOscillator();
      ~DSPOscillator();

      void setAmplitude(StkFloat amp);
      void setFrequency(StkFloat freq);
      void setWaveform(StkFloat wf);
      inline StkFloat tick();
      void reset();
};

inline StkFloat DSPOscillator::tick(){
   // std::cout << waveform << "\n";
   if(waveform < 0.5){
      saw->tick();
      out = amplitude * ( ( sine->tick() * (1.0 - 2.0*waveform) ) + ( 2 * waveform * square->tick() ) );
   }
   else{
      sine->tick();
      out = amplitude * ( ( 2 * (1.0-waveform) * square->tick() ) +  (saw->tick() * (waveform - 0.5) * 2.0) );
   }

   return out;
}
