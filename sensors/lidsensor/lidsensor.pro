CONFIG      += link_pkgconfig

TARGET       = lidsensor

HEADERS += lidsensor.h   \
           lidsensor_a.h \
           lidplugin.h

SOURCES += lidsensor.cpp   \
           lidsensor_a.cpp \
           lidplugin.cpp

include( ../sensor-config.pri )
