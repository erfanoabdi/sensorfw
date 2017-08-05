TARGET       = hybrispressureadaptor

HEADERS += hybrispressureadaptor.h \
           hybrispressureadaptorplugin.h

SOURCES += hybrispressureadaptor.cpp \
           hybrispressureadaptorplugin.cpp
LIBS+= -L../../core -lhybrissensorfw-qt5

include(../adaptor-config.pri )
config_hybris {
    INCLUDEPATH+=/usr/include/android
}
