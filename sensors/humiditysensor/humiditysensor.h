/**
   @file humiditysensor.h
   @brief humiditysensor

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

#ifndef HUMIDITY_SENSOR_CHANNEL_H
#define HUMIDITY_SENSOR_CHANNEL_H

#include <QObject>

#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "humiditysensor_a.h"
#include "dataemitter.h"
#include "datatypes/timedunsigned.h"
#include "datatypes/unsigned.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor for accessing the relative humidity sensor measurements.
 *
 * Signals listeners whenever observed relative humidity level has
 * changed.
 */
class HumiditySensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<TimedUnsigned>
{
    Q_OBJECT
    Q_PROPERTY(Unsigned relativeHumidity READ relativeHumidity NOTIFY relativeHumidityChanged)

public:
    /**
     * Factory method for HumiditySensorChannel.
     * @return New HumiditySensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        HumiditySensorChannel* sc = new HumiditySensorChannel(id);
        new HumiditySensorChannelAdaptor(sc);

        return sc;
    }

    /**
     * Property for accessing the measured value.
     * @return Last measured value.
     */
    Unsigned relativeHumidity() const { return previousRelativeValue_; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when a change in measured data is observed.
     * @param value Measured value.
     */
    void relativeHumidityChanged(const Unsigned& value);

protected:
    HumiditySensorChannel(const QString& id);
    virtual ~HumiditySensorChannel();

private:
    TimedUnsigned                 previousRelativeValue_;
    TimedUnsigned                 previousAbsoluteValue_;
    Bin*                          filterBin_;
    Bin*                          marshallingBin_;
    DeviceAdaptor*                humidityAdaptor_;
    BufferReader<TimedUnsigned>*  humidityReader_;
    RingBuffer<TimedUnsigned>*    outputBuffer_;

    void emitData(const TimedUnsigned& value);
};

#endif // HUMIDITY_SENSOR_CHANNEL_H
