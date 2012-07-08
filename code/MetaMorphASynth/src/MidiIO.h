#pragma once

#include "RtMidi.h"
#include "MidiMap.h"
#include "Constants.h"

class MidiIO{
	public:
      MidiIO(MidiMap* map);
      ~MidiIO();

      /**
       * Connects to default midi port and register callback to receive midi events
       */
      void start();

   private:
      RtMidiIn* midiIn;
      MidiMap* midiMap;

      /**
       * Receives midi messages from OS
       */
      void midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData);

      // Use member function the midi callback. See http://www.assembla.com/spaces/portaudio/wiki/Tips_CPlusPlus
      static void _midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData){
         return ((MidiIO*)userData)->midiCallback(deltatime, message, userData); 
      }
};
