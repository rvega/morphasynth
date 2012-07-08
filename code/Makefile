# Change the following according to your OS:
OF_RELEASE_URL = http://www.openframeworks.cc/versions/preRelease_v0.07/of_preRelease_v007_linux64.tar.gz
OF_INSTALL_SCRIPTS = libs/openFrameworks/scripts/linux/ubuntu

STK_RELEASE_URL = http://ccrma.stanford.edu/software/stk/release/stk-4.4.3.tar.gz

# The version of RtMidi included in the stk does not fully support Jack Midi
RTMIDI_RELEASE_URL = http://www.music.mcgill.ca/~gary/rtmidi/release/rtmidi-2.0.0.tar.gz

# # # # # # # # # # # # # # # # # # # # # # 
# Downloads dependencies and git submodules
# # # # # # # # # # # # # # # # # # # # # # 
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
