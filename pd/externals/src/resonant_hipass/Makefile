# For some reason this external stopped working completely. Don't compile it so we can keep using the old binaries while transitioning to the new resonant_hpf


# NAME = resonant_hipass
# 
# # Detect Linux or MacOS
# UNAME := $(shell uname -s)
# ifeq ($(UNAME),Darwin)
# 	EXT = pd_darwin
# endif
# ifeq ($(UNAME),Linux)
# 	EXT = pd_linux
# endif
# 
# all: $(NAME).dsp
# 	faust2puredata $(NAME).dsp
# 
# cpp:
# 	faust -a puredata.cpp -o $(NAME).cpp $(NAME).dsp
# 	sed -i '' 's/void faust_setup(/void $(NAME)_setup(/g' $(NAME).cpp
# 	echo "#define mydsp $(NAME)" > tmp && cat $(NAME).cpp >> tmp && rm $(NAME).cpp && mv tmp $(NAME).cpp
# 
# clean_cpp:
# 	rm -f $(NAME).cpp
# 
# clean:
# 	rm -f $(NAME)~.$(EXT)
# 
# uninstall:
# 	rm -f ../../bin/$(NAME)~.$(EXT)
# 	rm -f ../../bin/$(NAME)~-help.pd
# 	rm -f ../../bin/$(NAME)~-meta.pd
# 	
# install:
# 	cp -f $(NAME)~.$(EXT) ../../bin
# 	cp -f *.pd ../../bin
# 
# 
