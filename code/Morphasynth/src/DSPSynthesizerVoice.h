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

using namespace stk;

class DSPSynthesizerVoice{
   private:
      short int currentNote;
      bool silence;
      
      // DSP Blocks:
      DSPNoiseWithLevel* noise;
      DSPOscillator* oscillator1;
      //    DSPLoPass* lopass;
      //    DSPHiPass* hipass;
      ADSR* envelope;
      
      // Some intermediate signals:
      StkFloat output;

   public:
      DSPSynthesizerVoice();
      ~DSPSynthesizerVoice();

      void setSampleRate(unsigned int sampleRate);
      short int getCurrentNote();
      void setParameter(Parameter param, float value);
      void noteOn(short int noteNumber, short int velocity);
      void noteOff();
      void process(StkFloat* out, unsigned int bufferSize);
};
