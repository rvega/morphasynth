UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	EXT = pd_darwin
endif
ifeq ($(UNAME),Linux)
	EXT = pd_linux
endif

all: moog_lopass.dsp
	faust2puredata moog_lopass.dsp

clean:
	rm -f moog_lopass~.$(EXT)

install:
	cp -f moog_lopass~.$(EXT) ../../bin
	cp -f *.pd ../../bin
