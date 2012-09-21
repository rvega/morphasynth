#  Morphasynth
#  
#  Copyright (C) 2012 Rafael Vega González
#  Copyright (C) 2012 Daniel Gómez Marín
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

OF_RELEASE_URL = http://www.openframeworks.cc/versions/preRelease_v0.07/of_preRelease_v007_linux64.tar.gz
OF_INSTALL_SCRIPTS = libs/openFrameworks/scripts/linux/ubuntu
STK_RELEASE_URL = http://ccrma.stanford.edu/software/stk/release/stk-4.4.3.tar.gz
RTMIDI_RELEASE_URL = http://www.music.mcgill.ca/~gary/rtmidi/release/rtmidi-2.0.0.tar.gz # The version of RtMidi included in the stk does not fully support Jack Midi

download_dependencies: 
	sudo ls
	# Download, uncompress and compile openframeworks:
	wget -c $(OF_RELEASE_URL) -O of.tar.gz
	mkdir -p libs/openFrameworks
	tar -xvzf of.tar.gz -C libs/openFrameworks --strip-components=1
	cd $(OF_INSTALL_SCRIPTS) && sudo chmod +x ./* && sudo ./install_dependencies.sh && sudo ./install_codecs.sh
	
	#Download and uncompress the STK
	wget -c $(STK_RELEASE_URL) -O stk.tar.gz
	mkdir -p libs/stk
	tar -xvzf stk.tar.gz -C libs/stk --strip-components=1

	#Change some names that will clash between stk and of
	find libs/stk -type f -exec sed -i 's/ TWO_PI/ STK_TWO_PI/g' {} \;
	find libs/stk -type f -exec sed -i 's/(TWO_PI/(STK_TWO_PI/g' {} \;
	find libs/stk -type f -exec sed -i 's/ PI/ STK_PI/g' {} \;
	find libs/stk -type f -exec sed -i 's/(PI/(STK_PI/g' {} \;
	find libs/stk -type f -exec sed -i 's/*PI/*STK_PI/g' {} \;
	
	# Compile STK
	cd libs/stk && ./configure --with-jack && make

	#Download uncompress and compile RtMidi
	wget -c $(RTMIDI_RELEASE_URL) -O rtmidi.tar.gz
	mkdir -p libs/rtmidi
	tar -xvzf rtmidi.tar.gz -C libs/rtmidi --strip-components=1
	cd libs/rtmidi && ./configure --with-jack && make -j1

	# Download/update other stuff specified in .gitmodules
	cd .. && git submodule init && git submodule update
	
	#Cleanup
	rm of.tar.gz
	rm stk.tar.gz
	rm rtmidi.tar.gz
