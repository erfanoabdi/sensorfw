/**
   @file pressuresensor_i.h
   @brief Interface for PressureSensor

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

#ifndef PRESSURESENSOR_I_H
#define PRESSURESENSOR_I_H

#include <QtDBus/QtDBus>

#include "datatypes/unsigned.h"
#include "abstractsensor_i.h"

/**
 * Client interface for accessing pressure sensor.
 * Provides signal on change of measured pressure level.
 * Previous measured pressure level can be queried any time. Provided
 * values are in \e pascals.
 */
class PressureSensorChannelInterface : public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(PressureSensorChannelInterface)
    Q_PROPERTY(Unsigned pressure READ pressure NOTIFY pressureChanged)

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
     * Get latest pressure from sensor daemon.
     *
     * @return pressure reading.
     */
    Unsigned pressure();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session ID.
     */
    PressureSensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static const PressureSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static PressureSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static PressureSensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

Q_SIGNALS:
    /**
     * Sent when measured ambient light intensity has changed.
     *
     * @param value ambient light reading.
     */
    void pressureChanged(const Unsigned& value);
};

namespace local {
  typedef ::PressureSensorChannelInterface PressureSensor;
}

#endif
