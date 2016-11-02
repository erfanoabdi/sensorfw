/**
   @file humiditysensor_i.h
   @brief Interface for HumiditySensor

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

#ifndef HUMIDITYSENSOR_I_H
#define HUMIDITYSENSOR_I_H

#include <QtDBus/QtDBus>

#include "datatypes/unsigned.h"
#include "abstractsensor_i.h"

/**
 * Client interface for accessing ambient light sensor.
 * Provides signal on change of measured ambient light intensity level.
 * Previous measured intensity level can be queried any time. Provided
 * values are in \e lux.
 */
class HumiditySensorChannelInterface : public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(HumiditySensorChannelInterface)
    Q_PROPERTY(Unsigned relativeHumidity READ relativeHumidity NOTIFY relativeHumidityChanged)

public:
    /**
     * Name of the D-Bus interface for this class.
     */
    static const char* staticInterfaceName;

    /**
     * Create new instance of the class.
     *
     * @param id Sensor ID.
     * @param sessionId Session ID.
     * @return Pointer to new instance of the class.
     */
    static AbstractSensorChannelInterface* factoryMethod(const QString& id, int sessionId);

    /**
     * Get latest relative humidity reading from sensor daemon.
     *
     * @return relative humidity reading.
     */
    Unsigned relativeHumidity();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session ID.
     */
    HumiditySensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static const HumiditySensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static HumiditySensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static HumiditySensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

Q_SIGNALS:
    /**
     * Sent when measured relative humidity has changed.
     *
     * @param value relative humidity reading.
     */
    void relativeHumidityChanged(const Unsigned& value);
};

namespace local {
  typedef ::HumiditySensorChannelInterface HumiditySensor;
}

#endif
