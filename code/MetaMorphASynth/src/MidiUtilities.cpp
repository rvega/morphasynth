#include "MidiUtilities.h"

void MidiUtilities::initMidiNotes(){
   for(int i=0; i<127; i++){
      midiNotes[i] = 440.0 * pow(2.0, (i-69.0)/12.0);
   }
}

float MidiUtilities::midiNote2Pitch(short int midiNote){
   return midiNotes[midiNote];
}
