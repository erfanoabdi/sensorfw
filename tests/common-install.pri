QT += testlib

include(../common-config.pri)

QMAKE_LIBDIR_FLAGS += -L../../datatypes -L../../../datatypes

QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5

target.path = /usr/bin
INSTALLS += target
