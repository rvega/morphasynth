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
#include "RtAudio.h"

#include "Constants.h"
#include "DSPSynthesizer.h"

class AudioIO{
   protected:

      // Use member function process as the audio callback.
      // See http://www.assembla.com/spaces/portaudio/wiki/Tips_CPlusPlus
      int process(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData);
      static int audioIOCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData){
         return ((AudioIO*)userData)->process(outputBuffer, inputBuffer, nFrames, streamTime, status, userData); 
      }

      RtAudio* dac;
      DSPSynthesizer* synth;
      unsigned int sampleRate;

   public:
      AudioIO(DSPSynthesizer* synthesizer);
      ~AudioIO();

      void start();
      void stop();
};
