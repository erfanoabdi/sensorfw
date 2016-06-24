TEMPLATE = subdirs

include( ../common-config.pri )
include( ../common-install.pri )

SUBDIRS  = accelerometersensor \
           orientationsensor \
           tapsensor \
           alssensor \
           proximitysensor \
           compasssensor \
           rotationsensor \
           magnetometersensor \
           gyroscopesensor \
           lidsensor \
           humiditysensor \
           pressuresensor \
           temperaturesensor \
           stepcountersensor

contextprovider:SUBDIRS += contextplugin
