all: moog_lopass.dsp
	faust -a puredata.cpp -o obj/moog_lopass.cpp moog_lopass.dsp
	g++ -DPD -Wall -O3 -c -Dmydsp=moog_lopass -o bin/moog_lopass.a obj/moog_lopass.cpp
	g++ -DPD -Wall -fPIC -O3 -shared -Dmydsp=moog_lopass -o bin/moog_lopass~.pd_linux obj/moog_lopass.cpp
	chmod 775 bin/moog_lopass.a
	chmod 775 bin/moog_lopass~.pd_linux
clean:
	rm -f bin/moog_lopass.a
	rm -f bin/moog_lopass~.pd_linux
	rm -f obj/moog_lopass.cpp

install:
	cp bin/moog_lopass~.pd_linux ~/pd-externals
