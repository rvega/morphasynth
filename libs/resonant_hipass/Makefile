all: obj/resonant_hipass.cpp
	g++ -DPD -Wall -O3 -c -msse -Dmydsp=resonant_hipass -o obj/resonant_hipass.o obj/resonant_hipass.cpp
	ar rcu bin/resonant_hipass.a obj/resonant_hipass.o
	g++ -DPD -Wall -fPIC -O3 -shared -msse -Dmydsp=resonant_hipass -o bin/resonant_hipass~.pd_linux obj/resonant_hipass.cpp
	chmod 775 bin/resonant_hipass.a
	chmod 775 bin/resonant_hipass~.pd_linux

cpp: obj/resonant_hipass.cpp


obj/resonant_hipass.cpp:
	faust -a puredata.cpp -o obj/resonant_hipass.cpp resonant_hipass.dsp
	sed -i 's/PdUI/PdUIResonantHipass/g' obj/resonant_hipass.cpp

clean:
	rm -f obj/resonant_hipass.o
	rm -f bin/resonant_hipass.a
	rm -f bin/resonant_hipass~.pd_linux
	rm -f obj/resonant_hipass.cpp

install:
	cp bin/resonant_hipass~.pd_linux ~/pd-externals
