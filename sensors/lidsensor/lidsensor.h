/**
   @file alssensor.h
   @brief ALSSensor

   <p>
   Copyright (C) 2016 Canonical,  Ltd.

   @author Lorn Potter <lorn.potter@canonical.com>

   This file is part of Sensord.

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

#ifndef LID_SENSOR_CHANNEL_H
#define LID_SENSOR_CHANNEL_H

#include <QObject>

#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "lidsensor_a.h"
#include "dataemitter.h"
#include "datatypes/liddata.h"
#include "datatypes/lid.h"
class Bin;
template <class TYPE> class BufferReader;
class FilterBase;


/**
 * @brief Sensor for accessing the internal ambient light sensor measurements.
 *
 * Signals listeners whenever observed ambient light intensity level has
 * changed.
 */
class LidSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<LidData>
{
    Q_OBJECT
    Q_PROPERTY(LidData closed READ closed)

public:
    /**
     * Factory method for LidSensorChannel.
     * @return New LidSensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        LidSensorChannel* sc = new LidSensorChannel(id);
        new LidSensorChannelAdaptor(sc);

        return sc;
    }

    /**
     * Property for accessing the measured value.
     * @return Last measured value.
     */
    LidData closed() const { return previousValue_; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when a change in measured data is observed.
     * @param value Measured value.
     */
    void lidChanged(const Lid& value);

protected:
    LidSensorChannel(const QString& id);
    virtual ~LidSensorChannel();

private:
    LidData                 previousValue_;
    Bin*                          filterBin_;
    Bin*                          marshallingBin_;
    DeviceAdaptor*                lidAdaptor_;
    BufferReader<LidData>*  lidReader_;
    RingBuffer<LidData>*    outputBuffer_;

    void emitData(const LidData& value);

};

#endif // LID_SENSOR_CHANNEL_H
