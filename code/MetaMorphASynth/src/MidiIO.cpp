#include "MidiIO.h"

MidiIO::MidiIO(MidiMap* mmap):
   midiIn(new RtMidiIn(RtMidi::UNIX_JACK, APPLICATION_NAME)),
   midiMap(mmap)
{
   //Constructor
}

MidiIO::~MidiIO(){
   delete midiIn;
}

void MidiIO::start(){
   // // Debug. Show available ports and connect automatically
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

void MidiIO::midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData){
   midiMap->process(deltatime, message);
}
