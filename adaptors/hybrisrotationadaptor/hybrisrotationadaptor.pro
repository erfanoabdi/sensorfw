TARGET       = hybrisrotationadaptor

HEADERS += hybrisrotationadaptor.h \
           hybrisrotationadaptorplugin.h

SOURCES += hybrisrotationadaptor.cpp \
           hybrisrotationadaptorplugin.cpp

LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
config_hybris {
    PKGCONFIG += android-headers
}
