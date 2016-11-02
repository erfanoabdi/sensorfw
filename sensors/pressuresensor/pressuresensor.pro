CONFIG      += link_pkgconfig

TARGET       = pressuresensor

HEADERS += pressuresensor.h   \
           pressuresensor_a.h \
           pressureplugin.h

SOURCES += pressuresensor.cpp   \
           pressuresensor_a.cpp \
           pressureplugin.cpp

include( ../sensor-config.pri )

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

