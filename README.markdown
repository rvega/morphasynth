MORPHASYNTH
===========

An audio synthesizer written in C++, Pd and HTML/javascript.  
Currently only running as a jack client on Linux (please port).  
All the parameters in the synthesizer are continuous (no switches, only knobs), this will allow for interpolation of timbres. 

Linux Build Instructions
------------------------

### 0. Get dependencies

Tested in Ubuntu 12.04 only

    sudo apt-get install build-essential qt4-qmake qt4-dev-tools puredata-dev  libjack-dev

### 1. Get the code

    git clone https://github.com/rvega/morphasynth.git repo
    cd repo 
    git submodule init
    git submodule update

### 2. Compile it

    qmake 
    make

### 3. Run it

    make run

Interesting stuff in the code
-----------------------------

* How to build Pd externals easily with faust.
* Write dsp/synthesis algorithms in Pd and embed them in a C++ application.
* How to compile freely available externals for use with libpd.
* Build HTML/javascript interfaces and embed them into a C++ application.
* Build a jack audio application.

How to edit/debug the DSP code
------------------------------

Install pd-extended from http://puredata.info/downloads, install our externals and open the repo/res/pd/main_patch.pd file

To install our externals do this:
    
    cd repo/libs/moog_lopass
    make 
    make install

    cd repo/libs/resonant_hipass
    make 
    make install

    cd repo/libs/directory
    make 
    make install

You may need to add the ~/pd-externals and ~/pd-externals/directory paths to PD's path usinf the File->Path menu.

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

