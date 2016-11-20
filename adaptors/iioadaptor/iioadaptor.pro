TARGET = iioaccelerometeradaptor
#TARGET = iiosensorsadaptor

HEADERS += iioadaptor.h \
           iioadaptorplugin.h

SOURCES += iioadaptor.cpp \
           iioadaptorplugin.cpp

CONFIG += qt debug warn_on link_prl link_pkgconfig plugin

PKGCONFIG += sensord-qt5
for(PKG, $$list($$unique(PKGCONFIG))) {
     !system(pkg-config --exists $$PKG):error($$PKG development files are missing)
}

CONFIG += link_pkgconfig
PKGCONFIG += udev
LIBS += -ludev

include( ../adaptor-config.pri )
