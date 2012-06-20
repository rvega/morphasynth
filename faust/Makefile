all: 
	make LPF
	make HPF

HPF: HPF.dsp
	faust -a jack-gtk.cpp -o HPF.cpp HPF.dsp
	g++ -O3 HPF.cpp `pkg-config --cflags --libs jack gtk+-2.0` -I/usr/local/lib/faust -o HPF
	chmod +x HPF
	./HPF &

LPF: LPF.dsp
	faust -a jack-gtk.cpp -o LPF.cpp LPF.dsp
	g++ -O3 LPF.cpp `pkg-config --cflags --libs jack gtk+-2.0` -I/usr/local/lib/faust -o LPF
	chmod +x LPF
	./LPF &

clean:
	rm HPF.cpp
	rm HPF
	rm LPF.cpp
	rm LPF
