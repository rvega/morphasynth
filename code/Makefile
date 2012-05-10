# # # # # # # # #
# Compile the app
# # # # # # # # #
all: release

release: 
	cd MetaMorphASynth && make Release

debug:
	cd MetaMorphASynth && make Debug

clean:
	cd MetaMorphASynth && make clean

# # # # # # # # # # # # # # # # # # # # # # 
# Downloads dependencies and git submodules
# # # # # # # # # # # # # # # # # # # # # # 
download_dependencies: 
	# Download and uncompress openframeworks:
	wget http://www.openframeworks.cc/versions/preRelease_v0.07/of_preRelease_v007_linux64.tar.gz # You need to change this according to your OS
	tar -xvzf of_preRelease_v007_linux64.tar.gz
	mv of_preRelease_v007_linux64 libs/openFrameworks
	rm of_preRelease_v007_linux64.tar.gz
	
	#Download and uncompress the STK
	wget http://ccrma.stanford.edu/software/stk/release/stk-4.4.3.tar.gz
	tar -xvzf stk-4.4.3.tar.gz
	mv stk-4.4.3 libs/stk
	rm stk-4.4.3.tar.gz

	# Download/update other stuff specified in .gitmodules
	cd .. && git submodule init && git submodule update

install_dependencies:
	cd libs/stk && ./configure --with-jack && make
	
	# Change this according to your OS:
	cd libs/openFrameworks/scripts/linux/ubuntu && sudo chmod +x ./* && sudo ./install_dependencies.sh && sudo ./install_codecs.sh

