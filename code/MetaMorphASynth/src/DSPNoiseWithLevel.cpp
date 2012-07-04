#include "DSPNoiseWithLevel.h"

DSPNoiseWithLevel::DSPNoiseWithLevel():
   Noise(),
   level(0.0)
{
   //Constructor.
}

DSPNoiseWithLevel::~DSPNoiseWithLevel(){
   //Destructor.
}

void DSPNoiseWithLevel::setLevel(StkFloat level){
   if(level > 1.0) this->level = 1.0;
   else if(level < 0.0) this->level = 0.0;
   else this->level = level;
}
