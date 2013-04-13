MoogLopass
===========

This is a [PureData](http://puredata.info) external that implements a lopass filter similar to the one available in moog synthesizers.

Build Instructions
------------------

### For Linux and MacOS

1. Install the latest stable verion of [Faust](http://faust.grame.fr/). 

    wget http://sourceforge.net/projects/faudiostream/files/latest/download?source=files -O faust.zip
    unzip faust.zip -d ./
    cd faust-0.9.58
    make 
    sudo make install

2. Use Faust to generate the external

    faust2puredata moog_lopass.dsp

3. Install the external (optional, will install to ../bin)

    make install

### For Windows

I didn't find a practical way to build the external on Windows, go to the [Faust online compiler](http://faust.grame.fr/compiler/) and click your way around until you get your dll file.

### For libpd

Generate a cpp file using the included makefile and then include the cpp file in your libpd project

    make cpp

License
-------

Copyright (C) 2012 Rafael Vega González  
Copyright (C) 2012 Daniel Gómez Marín  

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
