CONFIG      += link_pkgconfig

TARGET       = temperaturesensor

HEADERS += temperaturesensor.h   \
           temperaturesensor_a.h \
           temperatureplugin.h

SOURCES += temperaturesensor.cpp   \
           temperaturesensor_a.cpp \
           temperatureplugin.cpp

include( ../sensor-config.pri )

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

