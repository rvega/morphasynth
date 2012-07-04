#include "DSPOscillator.h"
#include <math.h>

DSPOscillator::DSPOscillator(): 
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

DSPOscillator::~DSPOscillator(){
   delete sine;
   delete square;
   delete saw;
}

void DSPOscillator::setAmplitude(StkFloat amp){
   if(amp > 1.0) amp = 1.0;
   else if(amp < 0.0) amp = 0.0;
   amplitude = amp;
}

void DSPOscillator::setFrequency(StkFloat freq){
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


void DSPOscillator::setWaveform(StkFloat wf){
   if(wf > 1.0) wf = 1.0;
   else if(wf < 0.0) wf = 0.0;
   waveform = wf;
}

void DSPOscillator::reset(){
   sine->reset();
   square->reset();
   saw->reset();
   // sine->setPhase(0.0);
   // square->setPhase(0.0);
   // saw->setPhase(0.0);
}
