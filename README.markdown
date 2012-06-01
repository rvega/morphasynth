METAMORPHASYNTH
===============

A synthesizer written in C++ with Open Frameworks (OF), The Synthesis Toolkit (STK), ofxUI, RTAudio, jack_ring_buffer.

Currently only running as a jack client on Linux (please port). 

All the parameters in the synthesizer are continuous (no switches, only knobs), this will allow for interpolation of timbres. 

Linux Build Instructions
------------------------

cd /path/to/repo/code  
make download_dependencies  
make install_dependencies  
cd MetaMorphASynth  
make Debug  