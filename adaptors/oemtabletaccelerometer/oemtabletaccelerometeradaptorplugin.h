#ifndef OEMTABLETACCELEROMETERADAPTORPLUGIN_H
#define OEMTABLETACCELEROMETERADAPTORPLUGIN_H

#include "plugin.h"

class OemtabletAccelerometerAdaptorPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")

private:
    void Register(class Loader& l);
};

#endif
