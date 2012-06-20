#include "NoiseWithLevel.h"

NoiseWithLevel::NoiseWithLevel():
   Noise(),
   level(0.0)
{
   //Constructor.
}

NoiseWithLevel::~NoiseWithLevel(){
   //Destructor.
}

void NoiseWithLevel::setLevel(StkFloat level){
   if(level > 1.0) this->level = 1.0;
   else if(level < 0.0) this->level = 0.0;
   else this->level = level;
}
