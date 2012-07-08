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
