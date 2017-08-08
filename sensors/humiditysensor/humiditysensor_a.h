/**
   @file humiditysensor_a.h
   @brief D-Bus adaptor for HumiditySensor

   <p>
   Copyright (C) 2016 Canonical LTD.

   @author Lorn Potter <lorn.potter@canonical.com>

   This file is part of Sensorfw.

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

#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include <QtDBus/QtDBus>
#include <QObject>

#include "datatypes/unsigned.h"
#include "abstractsensor_a.h"

class HumiditySensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(HumiditySensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.HumiditySensor")
    Q_PROPERTY(Unsigned relativeHumidity READ relativeHumidity NOTIFY relativeHumidityChanged)

public:
    HumiditySensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Unsigned relativeHumidity() const;

Q_SIGNALS:
    void relativeHumidityChanged(const Unsigned& value);
};

#endif
