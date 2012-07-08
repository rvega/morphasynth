#include "MidiIO.h"

MidiIO::MidiIO(MidiMap* mmap):
   midiIn(new RtMidiIn(APPLICATION_NAME)),
   midiMap(mmap)
{
   //Constructor
}

MidiIO::~MidiIO(){
   delete midiIn;
}

void MidiIO::start(){
   unsigned int nPorts = midiIn->getPortCount();
   if ( nPorts == 0 ) {
      std::cout << "Could not connect to midi device" << "\n";
      return;
   }
   midiIn->openPort(0, "MIDI In");
   midiIn->setCallback(&MidiIO::_midiCallback, this);
}

void MidiIO::midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData){
   midiMap->process(deltatime, message);
}
