#pragma once

#include <iostream>

class Oscillator{
   protected:
      unsigned int sampleRate;

   public:
      Oscillator();
      ~Oscillator();

      inline void process(void *outputBuffer, void *inputBuffer);
};

void Oscillator::process(void *outputBuffer, void *inputBuffer){
   return;
}
