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

# Compile lowpass external and link statically
PRE_TARGETDEPS += libs/moog_lopass/bin/moog_lopass.a
lpf.target = libs/moog_lopass/bin/moog_lopass.a
lpf.commands = cd libs/moog_lopass && make
QMAKE_EXTRA_TARGETS += lpf
LIBS += libs/moog_lopass/bin/moog_lopass.a

# Compile expr external and link statically
PRE_TARGETDEPS += libs/libpd/pure-data/extra/expr~/expr.a
expr.target = libs/libpd/pure-data/extra/expr~/expr.a
expr.commands = cd libs/libpd/pure-data/extra/expr~ && make
QMAKE_EXTRA_TARGETS += expr
LIBS += libs/libpd/pure-data/extra/expr~/expr.a

# Compile libPd and link statically
PRE_TARGETDEPS += libs/libpd/libs/libpd.a
libpd.target = libs/libpd/libs/libpd.a
libpd.commands = cd libs/libpd && make
QMAKE_EXTRA_TARGETS += libpd
INCLUDEPATH += ./libs/libpd/libpd_wrapper ./libs/libpd/pure-data/src
LIBS += ./libs/libpd/libs/libpd.a

# Run
run.target = run
run.commands = cd bin && ./morphasynth
run.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += run
