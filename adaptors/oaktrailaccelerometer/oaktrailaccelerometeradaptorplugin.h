#ifndef OAKTRAILACCELEROMETERADAPTORPLUGIN_H
#define OAKTRAILACCELEROMETERADAPTORPLUGIN_H

#include "plugin.h"

class OaktrailAccelerometerAdaptorPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
private:
    void Register(class Loader& l);
};

#endif
