all: compile_run

compile_run:
	faust -a jack-gtk.cpp -o filter.cpp filter.dsp
	g++ -O3 filter.cpp `pkg-config --cflags --libs jack gtk+-2.0` -I/usr/local/lib/faust -o filter
	chmod +x filter
	./filter &

generate_dsp_code:
	faust -a minimal.cpp filter.dsp

clean:
	rm filter.cpp
	rm filter
