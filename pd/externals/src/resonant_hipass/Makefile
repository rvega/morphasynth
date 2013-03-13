UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	EXT = pd_darwin
endif
ifeq ($(UNAME),Linux)
	EXT = pd_linux
endif

all: resonant_hipass.dsp
	faust2puredata resonant_hipass.dsp

clean:
	rm -f resonant_hipass~.$(EXT)

install:
	cp -f resonant_hipass~.$(EXT) ../../bin
	cp -f *.pd ../../bin
