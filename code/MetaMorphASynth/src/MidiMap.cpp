#include "MidiMap.h"

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
   print(deltatime, bytes);

   // Parse the midi bytes.
   // First byte is XXXXYYYY where YYYY is the channel number and XXXX is the bytes type

   unsigned char channelNumber = bytes->at(0) & 0x0F;
   std::cout << "channel = 0x" << std::hex << (int)channelNumber << std::endl;

   unsigned char bytesType = bytes->at(0) & 0xF0;
   std::cout << "Message type = 0x" << std::hex << (int)bytesType << std::endl;

   // Note off can be 0x80 or 0x90 (note on) with velocity of zero
   if( bytesType==0x80 || (bytesType==0x90 && bytes->at(2)==0x00) ){
      std::cout << "Note off" << "\n";
   }
   // Note on
   else if(bytesType==0x90){
      std::cout << "Note on" << "\n";
   }
   // Pitch wheel
   else if(bytesType==0xE0){
      // Pitchwheel bytess use 2 bytes to express a value between -8192 and 8191
      // Byte 2 is the most significant one 
      short int value = ((bytes->at(2) << 7) | bytes->at(1)) - 8192;
      std::cout << "Pitch Wheel: " << std::dec << value << "\n";
   }
}
