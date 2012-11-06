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

# Compile limit external and link statically
PRE_TARGETDEPS += libs/maxlib/limit.a
limit.target = libs/maxlib/limit.a
limit.commands = cd libs/maxlib && make
QMAKE_EXTRA_TARGETS += limit
LIBS += libs/maxlib/limit.a

# Compile hppass external and link statically
PRE_TARGETDEPS += libs/resonant_hipass/bin/resonant_hipass.a
hpf.target = libs/resonant_hipass/bin/resonant_hipass.a
hpf.commands = cd libs/resonant_hipass && make
QMAKE_EXTRA_TARGETS += hpf
LIBS += libs/resonant_hipass/bin/resonant_hipass.a

# Compile lowpass external and link statically
PRE_TARGETDEPS += libs/moog_lopass/bin/moog_lopass.a
lpf.target = libs/moog_lopass/bin/moog_lopass.a
lpf.commands = cd libs/moog_lopass && make
QMAKE_EXTRA_TARGETS += lpf
LIBS += libs/moog_lopass/bin/moog_lopass.a

# Compile list2symbol external and link statically
PRE_TARGETDEPS += libs/zexy/src/list2symbol.a
list2symbol.target = libs/zexy/src/list2symbol.a
list2symbol.commands = cd libs/zexy/src && make
QMAKE_EXTRA_TARGETS += list2symbol
LIBS += libs/zexy/src/list2symbol.a

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


  
