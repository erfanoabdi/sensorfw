TARGET       = hybrisstepcounteradaptor

HEADERS += hybrisstepcounteradaptor.h \
           hybrisstepcounteradaptorplugin.h

SOURCES += hybrisstepcounteradaptor.cpp \
           hybrisstepcounteradaptorplugin.cpp
LIBS+= -L../../core -lhybrissensorfw-qt5

include(../adaptor-config.pri )
config_hybris {
    INCLUDEPATH+=/usr/include/android
}
