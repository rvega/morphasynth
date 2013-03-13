# QMake and QT config
CONFIG += qt debug threaded
QT += webkit
TEMPLATE = app

# Optimize harder, use c++ 2011 standard
QMAKE_CXXFLAGS += -O3 -std=c++0x

# Input
HEADERS += src/*.hpp
SOURCES += src/*.cpp

# Output
TARGET = ./bin/morphasynth

# Directory structure
DEPENDPATH += .
OBJECTS_DIR += ./obj
RCC_DIR += ./obj
MOC_DIR += ./obj

# Copy reources
res.path = bin/res
res.files = ../../html
res.files += ../../pd/patches
INSTALLS += res

# Link with jack libraries
LIBS += -ljack

EXTERNALS_PATH = ../../vendors/pd-extended-0.43.4/externals

# Compile limit external
SOURCES += $${EXTERNALS_PATH}/maxlib/limit.c

# Compile list2symbol external and link statically
SOURCES += $${EXTERNALS_PATH}/zexy/src/zexy.h
SOURCES += $${EXTERNALS_PATH}/zexy/src/zexy.c
SOURCES += $${EXTERNALS_PATH}/zexy/src/list2symbol.c

# Compile expr external and link statically
DEFINES += PD
SOURCES += $${EXTERNALS_PATH}/extra/expr~/*.h
SOURCES += $${EXTERNALS_PATH}/extra/expr~/*.c
# PRE_TARGETDEPS += libs/libpd/pure-data/extra/expr~/expr.a
# expr.target = libs/libpd/pure-data/extra/expr~/expr.a
# expr.commands = cd libs/libpd/pure-data/extra/expr~ && make
# QMAKE_EXTRA_TARGETS += expr
# LIBS += libs/libpd/pure-data/extra/expr~/expr.a

# Compile hppass external and link statically
# PRE_TARGETDEPS += libs/resonant_hipass/bin/resonant_hipass.a
# hpf.target = libs/resonant_hipass/bin/resonant_hipass.a
# hpf.commands = cd libs/resonant_hipass && make
# QMAKE_EXTRA_TARGETS += hpf
# LIBS += libs/resonant_hipass/bin/resonant_hipass.a

# Compile lowpass external and link statically
# PRE_TARGETDEPS += libs/moog_lopass/bin/moog_lopass.a
# lpf.target = libs/moog_lopass/bin/moog_lopass.a
# lpf.commands = cd libs/moog_lopass && make
# QMAKE_EXTRA_TARGETS += lpf
# LIBS += libs/moog_lopass/bin/moog_lopass.a


# Compile libPd and link statically
PRE_TARGETDEPS += ../../vendors/libpd/libs/libpd.a
libpd.target = ../../vendors/libpd/libs/libpd.a
libpd.commands = cd ../../vendors/libpd && make
QMAKE_EXTRA_TARGETS += libpd
INCLUDEPATH += ../../vendors/libpd/libpd_wrapper ../../vendors/libpd/pure-data/src
LIBS += ../../vendors/libpd/libs/libpd.a

# Run
run.target = run
run.commands = make install && cd bin && ./morphasynth
run.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += run


# vim: set ft=make:
