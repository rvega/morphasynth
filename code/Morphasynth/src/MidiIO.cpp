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

#include "MidiIO.h"

MidiIO::MidiIO(MidiMap* mmap):
   midiMap(mmap)
{
   // Constructor
}

MidiIO::~MidiIO(){
   delete midiIn;
}

void MidiIO::stop(){
   if(midiIn != 0){
      midiIn->closePort();
   }
}

void MidiIO::start(){
   try{
      midiIn = new RtMidiIn(RtMidi::UNIX_JACK, APPLICATION_NAME);

      // Debug. Show available ports and connect automatically
      // unsigned int nPorts = midiIn->getPortCount();
      // if ( nPorts == 0 ) {
      //    std::cout << "Could not connect to midi device" << "\n";
      //    return;
      // }
      // std::cout << "Available Midi ports:" << "\n";
      // for (int i = 0; i < nPorts; i++) {
      //    std::string name = midiIn->getPortName(i);
      //    std::cout << "(" << i << ") " << name << "\n";
      // }
      // midiIn->openPort(2, "MIDI In");
      
      midiIn->openVirtualPort("MIDI In");
      midiIn->setCallback(&MidiIO::_midiCallback, this);
   }
   catch(RtError& e){
      midiIn = 0;
      std::cout << "Could not start midi port. Is Jack running?." << "\n";
      std::cout << e.getMessage() << "\n";
   }
}

void MidiIO::midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData){
   int s = message->size();
   std::cout << s << " ";
   // for(int i=0; i<message->size(); i++) {
   //    std::cout << message->at(i) << " ";
   // }
   // std::cout << "\n";

   midiMap->process(deltatime, message);
}
