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

#include <iostream>

enum Parameter {
#  define X(a) a,
#  include "Parameter.def"
#  undef X
   ParamsCount
};

char const* const Parameter2String[] = {
#  define X(a) #a,
#  include "Parameter.def"
#  undef X
};

inline Parameter String2Parameter(std::string str){
   for (int i = 0; i < ParamsCount; i++) {
      if(Parameter2String[i]==str){
         return (Parameter)i;
      }
   }
   return ParamsCount;
};

inline std::ostream& operator<<(std::ostream& os, enum Parameter c) {
   if (c >= ParamsCount || c < 0) return os << "???";
   return os << Parameter2String[c];
}
