/**
 * Morphasynth
 * 
 * Copyright (C) 2012 Rafael Vega González
 * Copyright (C) 2012 Daniel Gómez Marín
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
