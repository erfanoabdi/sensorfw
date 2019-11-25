#ifndef OEMTABLETMAGNETOMETERADAPTOR_PLUGIN_H
#define OEMTABLETMAGNETOMETERADAPTOR_PLUGIN_H

#include "plugin.h"

class OemtabletMagnetometerAdaptorPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
private:
    void Register(class Loader& l);
};

#endif
