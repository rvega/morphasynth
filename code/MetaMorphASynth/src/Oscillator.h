#pragma once



#include "Stk.h"
#include "SineWave.h"
#include "BlitSquare.h"
#include "BlitSaw.h"

using namespace stk;

class Oscillator{
   protected:
      StkFloat amplitude;
      StkFloat frequency;
      StkFloat waveform;

      SineWave* sine;
      BlitSquare* square;
      BlitSaw* saw;

   public:
      Oscillator();
      ~Oscillator();

      void setAmplitude(StkFloat amp);
      void setFrequency(StkFloat freq);
      void setWaveform(StkFloat wf);
      inline StkFloat tick();
};

inline StkFloat Oscillator::tick(){
   // std::cout << waveform << "\n";
   if(waveform < 0.5){
      saw->tick();
      return amplitude * ( ( sine->tick() * (1.0 - 2.0*waveform) ) + ( 2 * waveform * square->tick() ) );
   }
   else{
      sine->tick();
      return amplitude * ( ( 2 * (1.0-waveform) * square->tick() ) +  (saw->tick() * (waveform - 0.5) * 2.0) );
   }
   // return amplitude * sine->tick();
   // return amplitude * square->tick();
   // return amplitude * saw->tick();
}
