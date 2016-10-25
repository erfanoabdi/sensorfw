/**
   @file lidsensoradaptor-evdev.h
   @brief LidSensorAdaptor utilizing evdev kernel interface

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

#ifndef LIDADAPTOR_EVDEV_H
#define LIDADAPTOR_EVDEV_H

#include "inputdevadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/liddata.h"

/**
 * @brief Adaptor for lid sensor.
 *
 * Events are received through @e evdev kernel interface.
 *
 * Listening clients are notified whenever the state has changed.
 */
class LidSensorAdaptorEvdev : public InputDevAdaptor
{
    Q_OBJECT
public:
//    /* Enumerates the possible states for proximity */
//    enum ProximityState {
//        ProximityStateUnknown = -1, /**< Proximity state unknown */
//        ProximityStateOpen = 0,     /**< Proximity open (no target detected)*/
//        ProximityStateClosed = 1    /**< Proximity closed (target nearby)*/
//    };

    /**
     * Factory method for gaining a new instance of LidsensorAdaptorEvdev class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new LidSensorAdaptorEvdev(id);
    }
    virtual bool startSensor();

    virtual void stopSensor();

    virtual bool standby();

    virtual bool resume();
    virtual void init();
protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    LidSensorAdaptorEvdev(const QString& id);
    ~LidSensorAdaptorEvdev();

private:

    DeviceAdaptorRingBuffer<LidData> *lidBuffer_;
    LidData::Type currentType_;
    LidData::Type lastType;
    qreal currentValue_;
    qreal lastValue;
    bool usingFront;

    void interpretEvent(int src, struct input_event *ev);
    void commitOutput(struct input_event *ev);
    void interpretSync(int src, struct input_event *ev);
    QByteArray powerStatePath_;
};

#endif
