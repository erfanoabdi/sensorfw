/**
   @file temperaturesensor.h
   @brief TemperatureSensor

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

#ifndef TEMPERATURE_SENSOR_CHANNEL_H
#define TEMPERATURE_SENSOR_CHANNEL_H

#include <QObject>

#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "temperaturesensor_a.h"
#include "dataemitter.h"
#include "datatypes/timedunsigned.h"
#include "datatypes/unsigned.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor for accessing the internal ambient light sensor measurements.
 *
 * Signals listeners whenever observed ambient light intensity level has
 * changed.
 */
class TemperatureSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<TimedUnsigned>
{
    Q_OBJECT
    Q_PROPERTY(Unsigned temperature READ temperature NOTIFY temperatureChanged)

public:
    /**
     * Factory method for TemperatureSensorChannel.
     * @return New TemperatureSensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        TemperatureSensorChannel* sc = new TemperatureSensorChannel(id);
        new TemperatureSensorChannelAdaptor(sc);

        return sc;
    }

    /**
     * Property for accessing the measured value.
     * @return Last measured value.
     */
    Unsigned temperature() const { return previousValue_; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when a change in measured data is observed.
     * @param value Measured value.
     */
    void temperatureChanged(const Unsigned& value);

protected:
    TemperatureSensorChannel(const QString& id);
    virtual ~TemperatureSensorChannel();

private:
    TimedUnsigned                 previousValue_;
    Bin*                          filterBin_;
    Bin*                          marshallingBin_;
    DeviceAdaptor*                temperatureAdaptor_;
    BufferReader<TimedUnsigned>*  temperatureReader_;
    RingBuffer<TimedUnsigned>*    outputBuffer_;

    void emitData(const TimedUnsigned& value);
};

#endif // TEMPERATURE_SENSOR_CHANNEL_H
