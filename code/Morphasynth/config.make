# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../libs/openFrameworks


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

USER_CFLAGS = -I../libs/rtmidi -I../libs/stk/include -Isrc/ringbuffer -Wno-sign-compare 


# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LD_FLAGS = libs/libawesomelib.a

USER_LDFLAGS = ../libs/rtmidi/librtmidi.a ../libs/stk/src/libstk.a 


# use this to add system libraries for example:
# USER_LIBS = -lpango
 
USER_LIBS = 


# change this to add different compiler optimizations to your project

# Rvega: -O3 is important to turn on function inlining: http://stackoverflow.com/questions/934529/c-inline-functions-using-gcc-why-the-call
USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -O3


EXCLUDE_FROM_SOURCE="bin,.xcodeproj,obj"

run:
	# make cleanDebug
	make Debug
	cd bin && ./Morphasynth_debug
