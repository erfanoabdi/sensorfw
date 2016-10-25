TEMPLATE = lib
TARGET = sensordatatypes

include( ../common-config.pri )

SENSORFW_INCLUDEPATHS = .. \
    ../filters

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += xyz.h \
    orientation.h \
    unsigned.h \
    magneticfield.h \
    compass.h \
    datarange.h \
    utils.h \
    timedunsigned.h \
    genericdata.h \
    orientationdata.h \
    tap.h \
    posedata.h \
    tapdata.h \
    touchdata.h \
    proximity.h \
    lid.h \
    liddata.h

SOURCES += xyz.cpp \
    orientation.cpp \
    unsigned.cpp \
    compass.cpp \
    utils.cpp \
    tap.cpp \
    lid.cpp

include(../common-install.pri)
publicheaders.path  = $${publicheaders.path}/datatypes
publicheaders.files = $$HEADERS
target.path = $$SHAREDLIBPATH
INSTALLS += target
