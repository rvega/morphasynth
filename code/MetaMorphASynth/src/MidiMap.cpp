#include "MidiMap.h"
#include "Controller.h"

MidiMap::MidiMap(Controller* ctlr): controller(ctlr) {
   //Constructor
}

MidiMap::~MidiMap(){

}

void MidiMap::print(double deltatime, std::vector< unsigned char > *bytes){
   unsigned int nBytes = bytes->size();
   for ( unsigned int i=0; i<nBytes; i++ )
      std::cout << "Byte " << i << " = " << (int)bytes->at(i) << ", ";
   if ( nBytes > 0 )
      std::cout << "stamp = " << deltatime << std::endl;
}

void MidiMap::process(double deltatime, std::vector< unsigned char > *bytes){
   // print(deltatime, bytes);

   // Parse the midi bytes.
   // First byte is XXXXYYYY where YYYY is the channel number and XXXX is the bytes type

   unsigned char channelNumber = bytes->at(0) & 0x0F;
   unsigned char messageType = bytes->at(0) & 0xF0;
   MidiEvent event;

   // Note off can be 0x80 or 0x90 (note on) with velocity of zero
   if( messageType==0x80 || (messageType==0x90 && bytes->at(2)==0x00) ){
      event.command = NOTE_OFF;
      event.value1 = bytes->at(1);
   }
   // Note on
   else if(messageType==0x90){
      event.command = NOTE_ON;
      event.value1 = bytes->at(1);
   }
   // Pitch wheel
   else if(messageType==0xE0){
      // Pitchwheel bytess use 2 bytes to express a value between -8192 and 8191
      // Byte 2 is the most significant one 
      event.command = PITCH_BEND;
      event.value1 = ((bytes->at(2) << 7) | bytes->at(1)) - 8192;
   }
   // Rvega: The only knob in my midi controller, I'll map it to something...
   // else if(messageType==0xB0 && bytes->at(1)==0x34){
   //    event.command = CUSTOM;
   //    value = bytes->at(2);
   // }
   
   controller->addMidiEvent(event);
}
