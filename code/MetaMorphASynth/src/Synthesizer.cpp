#include "Synthesizer.h"
#include <iostream>

Synthesizer::Synthesizer(): 
   sampleRate(0),
   outputBuffer(NULL),
   out(0.0),
   sine(new SineWave()),
   freq(0.0)
   {
      sine->setFrequency(0.0);
}

Synthesizer::~Synthesizer(){
   delete sine;
}

void Synthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate );
}

int Synthesizer::process(void *outBuffer, void *inBuffer, unsigned int bufferSize){
   outputBuffer = (StkFloat *)outBuffer;   

   freq = freq + 0.2;
   sine->setFrequency(freq);

   // std::cout << bufferSize << "\n";
   // Write interleaved audio data. We only calculate one channel and output the result twice. Once for each channel. We only calculate one channel and output the result twice. Once for each channel.
   for(unsigned int i = 0; i < bufferSize; i++) {
      out = sine->tick();
      // std::cout << out << "\n";
      *outputBuffer++ = out;
      *outputBuffer++ = out;
   }
   return 0;
}
