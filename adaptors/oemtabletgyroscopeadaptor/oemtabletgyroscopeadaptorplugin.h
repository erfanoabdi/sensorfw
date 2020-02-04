#ifndef OEMTABLETGYROSCOPEADAPTORPLUGIN_H
#define OEMTABLETGYROSCOPEADAPTORPLUGIN_H

#include "plugin.h"

class OEMTabletGyroscopeAdaptorPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
private:
    void Register(class Loader& l);
};

#endif
