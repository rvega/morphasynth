#include <iostream>
#include <cmath>

class MidiUtilities {
   public:
      void initMidiNotes();
      float midiNote2Pitch(short int midiNote);

   private:
      float midiNotes[127];
};

