#ifndef OEMTABLETALSADAPTOR_ASCIIPLUGIN_H
#define OEMTABLETALSADAPTOR_ASCIIPLUGIN_H

#include "plugin.h"

class OEMTabletALSAdaptorAsciiPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
private:
        void Register(class Loader& l);
};

#endif
