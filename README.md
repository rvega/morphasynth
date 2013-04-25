MORPHASYNTH
===========

A musical audio synthesizer written in [Pd](http://puredata.info/) and HTML+JavaScript. This is an exploration of preset morphing (smooth interpolation) using dimensionality reduction with audio descriptors as a means of simplifying user interfaces for setting timbres.

Some wrappers and scripts are provided for compiling as a Linux Jack client (currently broken), an iPad application (currently incomplete) and running the synthesizer using a web browser as the GUI and [pd-extended](http://puredata.info/downloads/pd-extended) as the sound engine.

Running in Pd only
------------------

Tested in pd-extended 0.43.4

There is a simple GUI implemented in PureData. Just open the pd/patches/00_Morphasynth_GUI.pd patch and start playing. We'll probably deprecate this at some point in favour of the HTML GUI.


Running in development mode (HTML GUI in Web Browser, DSP engine in pd-extended)
--------------------------------------------------------------------------------

Tested using Google Chrome 26, pd-extended 0.43.4, Ubuntu Linux 12.04, Windows XP, MacOS 10.7.5, node 0.10.5

### For the impatient:

1. Install the dependencies as described below (you only need to do this once).
2. Start the scripts/development.sh script.
3. Open html/index.html in a web browser. 
4. Open pd/patches/00_Morphasynth_NO_GUI.pd in pd-extended. 
5. Start hacking.

### Longer version:

The most practical way to hack or modify the code is to run and debug the HTML+JavaScript code using a web browser, open the Pd patches using pd-extended and use the provided script to allow for communication between both. You'll need to install some stuff first:

#### Custom Pd externals

In Pd parlance, an external is a plug-in or an extension to the Pd language. They are usually built by people outside the core Pd team. We have included some 3rd party externals and built a few ourselves for this project and you'll need to either use the pre-compiled versions included in this repository or compile them yourself.

If you want to use the pre-compiled versions (recommended), all you need to do is open pd-extended, go to Edit -> Preferences and add pd/externals/bin, pd/externals/bin/tID and pd/externals/bin/directory to Pd's search path.

If you want to modify the externals, take a look at the different readme files in the subdirectories of pd/externals/src.

#### Development script

To allow communication between the JavaScript GUI code running in a web browser and the Pd sound engine running in pd-extended. You need to run the development.sh (Linux and MacOS) or development.bat (Windows) scripts. To do so, you need to install some stuf first:

1. Install [node](http://nodejs.org/), [nmp](https://npmjs.org/) and [socket.io](http://socket.io/).

    In Ubuntu Linux:
  
        sudo add-apt-repository ppa:chris-lea/node.js  
        sudo apt-get update
        sudo apt-get install nodejs
        cd vendors/KievII/comm/oscproxy
        npm install socket.io

    In MacOS with [HomeBrew](http://mxcl.github.io/homebrew/)
        
        brew install node
        curl https://npmjs.org/install.sh | sh
        cd vendors/KievII/comm/oscproxy
        npm install socket.io

    In MacOS without HomeBrew (If you don't know/care what HomeBrew is), download and run the MacOS installer from [here](http://nodejs.org/download/) and then:
        
        cd vendors/KievII/comm/oscproxy
        npm install socket.io
            
    In Windows, download and run the Windows installer from [here](http://nodejs.org/download/) and then:

        cd vendors/KievII/comm/oscproxy
        npm install socket.io
        
2. Run it
        
    In Linux and MacOS:
    
        cd path/to/this/repo
        scripts/development.sh
    
    In Windows:

        cd path/to/this/repo
        scripts/development.bat

Building the iPad app
---------------------

Tested in MacOS 10.7, Xcode 4.6 and iOS 6.0

Open the wrappers/ios/Morphasynth.xcodeproj project in Xcode and click Run.


Building the Linux Jack Client
------------------------------

**This section is outdated and probably broken**

Tested in Ubuntu 12.04 with Jack packages from [KSStudio repositories](http://kxstudio.sourceforge.net/KXStudio:Repositories).

### 0. Get dependencies

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
* How to build Pd externals from scratch with C.
* Write dsp/synthesis algorithms in Pd and embed them in a C++ application.
* How to compile freely available externals for use with libpd.
* Build HTML/javascript interfaces and embed them into a C++ application.
* Build a jack audio application.

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

