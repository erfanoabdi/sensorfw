CONFIG      += link_pkgconfig

TARGET       = stepcountersensor

HEADERS += stepcountersensor.h   \
           stepcountersensor_a.h \
           stepcounterplugin.h

SOURCES += stepcountersensor.cpp   \
           stepcountersensor_a.cpp \
           stepcounterplugin.cpp

include( ../sensor-config.pri )

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

