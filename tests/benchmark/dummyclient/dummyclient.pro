TEMPLATE = app
TARGET = sensordummyclient
QT += dbus network

include( ../../common-install.pri)

INCLUDEPATH += ../../../qt-api \
               ../../../core \
               ../../../include \
               ../../..

SOURCES += dummyclient.cpp
HEADERS += dummyclient.h

QMAKE_LIBDIR_FLAGS += -L../../../qt-api  \
                      -L../../../datatypes \
                      -L../../../core

QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5 -lsensorclient-qt5
