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

#include "MidiUtilities.h"

MidiUtilities::MidiUtilities(){
   initMidiNotes();
}

void MidiUtilities::initMidiNotes(){
   for(int i=0; i<127; i++){
      midiNotes[i] = 440.0 * pow(2.0, (i-69.0)/12.0);
   }
}

float MidiUtilities::midiNote2Pitch(short int midiNote){
   return midiNotes[midiNote];
}
