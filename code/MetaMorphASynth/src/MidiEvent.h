#pragma once

#include "Command.h"
   
typedef struct MidiEvent{
   Command command;
   short int value1;   
   short int value2;   
} MidiEvent;
