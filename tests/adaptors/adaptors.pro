QT += testlib dbus network
QT -= gui

include(../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensoradaptors-test

HEADERS += adaptortest.h \
       ../../datatypes/utils.h \
    ../../adaptors/accelerometeradaptor/accelerometeradaptor.h \
    ../../adaptors/magnetometeradaptor/magnetometeradaptor.h \
    ../../adaptors/alsadaptor/alsadaptor.h \
    ../../adaptors/tapadaptor/tapadaptor.h \
    ../../adaptors/touchadaptor/touchadaptor.h \
    ../../adaptors/kbslideradaptor/kbslideradaptor.h \
    ../../adaptors/proximityadaptor/proximityadaptor.h \
    ../../adaptors/gyroscopeadaptor/gyroscopeadaptor.h \
    ../../adaptors/lidsensoradaptor-evdev/lidsensoradaptor-evdev.h

SOURCES += adaptortest.cpp \
    ../../datatypes/utils.cpp \
    ../../adaptors/accelerometeradaptor/accelerometeradaptor.cpp \
    ../../adaptors/magnetometeradaptor/magnetometeradaptor.cpp \
    ../../adaptors/alsadaptor/alsadaptor.cpp \
    ../../adaptors/tapadaptor/tapadaptor.cpp \
    ../../adaptors/touchadaptor/touchadaptor.cpp \
    ../../adaptors/kbslideradaptor/kbslideradaptor.cpp \
    ../../adaptors/proximityadaptor/proximityadaptor.cpp \
    ../../adaptors/gyroscopeadaptor/gyroscopeadaptor.cpp \
    ../../adaptors/lidsensoradaptor-evdev/lidsensoradaptor-evdev.cpp


INCLUDEPATH += ../.. \
    ../../include \
    ../../core \
    ../../datatypes \
    ../../filters \
    ../../adaptors/sysfsadaptor \
    ../../adaptors/touchadaptor \
    ../../adaptors/tapadaptor \
    ../../adaptors/alsadaptor \
    ../../adaptors/magnetometeradaptor \
    ../../adaptors/accelerometeradaptor \
    ../../adaptors/kbslideradaptor \
    ../../adaptors/proximityadaptor \
    ../../adaptors/gyroscopeadaptor \
    ../../adaptors/lidsensoradaptor-evdev


QMAKE_LIBDIR_FLAGS += -L../../builddir/core -L../../core/ -lrt

include(../../common.pri)
