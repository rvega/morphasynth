# QMake and QT config
CONFIG += qt debug threaded
QT += webkit
TEMPLATE = app

# Optimize harder, use c++ 2011 standard
QMAKE_CXXFLAGS += -O3 -std=c++0x

# Input
HEADERS += src/*.hpp
SOURCES += src/*.cpp
RESOURCES += res/resources.qrc

# Output
TARGET = ./bin/morphasynth

# Directory structure
DEPENDPATH += .
OBJECTS_DIR += ./obj
RCC_DIR += ./obj
MOC_DIR += ./obj

# Link with jack libraries
LIBS += -ljack

# Link libPd statically
INCLUDEPATH += ./libs/libpd/libpd_wrapper ./libs/libpd/pure-data/src
LIBS += ./libs/libpd/libs/libpd.a

# Compile lowpass external from faust to cpp
PRE_TARGETDEPS += src/externals/moog_lopass/LPF.cpp
lpf.target = src/externals/moog_lopass/LPF.cpp
lpf.commands = cd faust && make
QMAKE_EXTRA_TARGETS += lpf

# Compile lowpass external cpp into a linkable binary (.o)
PRE_TARGETDEPS += src/externals/moog_lopass/LPF.o
lpf2.target = src/externals/moog_lopass/LPF.o
lpf2.commands = g++ -O3 -c -fPIC -DPD -Wall -Dmydsp=moog_lopass -o obj/LPF.o src/externals/moog_lopass/LPF.cpp
lpf2.depends = src/externals/moog_lopass/LPF.cpp
QMAKE_EXTRA_TARGETS += lpf2
LIBS += obj/LPF.o


# Compile expr external
PRE_TARGETDEPS += src/externals/expr/vexp.pd_linux_o
expr.target = src/externals/expr/vexp.pd_linux_o
expr.commands = cd src/externals/expr && make
QMAKE_EXTRA_TARGETS += expr
LIBS += src/externals/expr/vexp_fun.pd_linux_o src/externals/expr/vexp_if.pd_linux_o src/externals/expr/vexp.pd_linux_o

# Compile libPd beforehand
PRE_TARGETDEPS += libs/libpd/libs/libpd.a
libpd.target = libs/libpd/libs/libpd.a
libpd.commands = cd libs/libpd && make
QMAKE_EXTRA_TARGETS += libpd

# Run
run.target = run
run.commands = cd bin && ./morphasynth
run.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += run
