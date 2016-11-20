CONFIG      += link_pkgconfig

TARGET       = humiditysensor

HEADERS += humiditysensor.h   \
           humiditysensor_a.h \
           humidityplugin.h

SOURCES += humiditysensor.cpp   \
           humiditysensor_a.cpp \
           humidityplugin.cpp

include( ../sensor-config.pri )

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

