all: resonant_hipass.dsp
	faust -a puredata.cpp -o obj/resonant_hipass.cpp resonant_hipass.dsp
	g++ -DPD -Wall -O3 -c -Dmydsp=resonant_hipass -o bin/resonant_hipass.a obj/resonant_hipass.cpp
	g++ -DPD -Wall -fPIC -O3 -shared -Dmydsp=resonant_hipass -o bin/resonant_hipass~.pd_linux obj/resonant_hipass.cpp
	chmod 775 bin/resonant_hipass.a
	chmod 775 bin/resonant_hipass~.pd_linux

clean:
	rm -f bin/resonant_hipass.a
	rm -f bin/resonant_hipass~.pd_linux
	rm -f obj/resonant_hipass.cpp

install:
	cp bin/resonant_hipass~.pd_linux ~/pd-externals
