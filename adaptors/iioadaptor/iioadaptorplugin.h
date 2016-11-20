/**
   @file iioadaptorplugin.h
   @brief Plugin for IioAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2012 Tuomas Kulve
   Copyright (C) 2016 Canonical

   @author Tuomas Kulve <tuomas@kulve.fi>
   @author Lorn Potte
r <lorn.potter@canonical.com>
   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/

#ifndef IIOADAPTORPLUGIN_H
#define IIOADAPTORPLUGIN_H

#include <plugin.h>

class IioAdaptorPlugin : public Plugin
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
#endif

private:
    void Register(class Loader& l);
};

#endif
