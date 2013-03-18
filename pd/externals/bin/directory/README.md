Directory
=========

This is a [PureData](http://puredata.info) external that allows you to list the contents of a directory in the filesystem. The interface is similar to [textfile].

Build Instructions
------------------

1. Install the latest stable verion of pd-extended as described [here](http://puredata.info/downloads/pd-extended). On Windows, use the installer, not the zip file. On Ubuntu Linux, check [this](http://puredata.info/docs/faq/debian) out.

2. Install the build tools for your platform. 

    On Ubuntu Linux, do

        sudo apt-get install build-essential

    On MacOs, you need something called "Command Line Tools For Xcode", available [here](http://developer.apple.com/downloads). You'll need to create a free developer account.

    On Windows, install [MinGW](www.mingw.org).

3. Compile and install. Using a terminal on Linux or MacOS, or the MinGW shell in Windows, do

    cd /path/to/this/directory
    make
    make install #(optional, will try to copy the compiled external to ../../bin)

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


