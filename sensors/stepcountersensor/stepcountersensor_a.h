/**
   @file stepcountersensor_a.h
   @brief D-Bus adaptor for stepcounter sensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2016 kimmoli

   @author Kimmo Lindholm <kimmo.lindholm@eke.fi>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef STEPCOUNTER_SENSOR_H
#define STEPCOUNTER_SENSOR_H

#include <QtDBus/QtDBus>
#include <QObject>

#include "datatypes/unsigned.h"
#include "abstractsensor_a.h"

class StepCounterSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(StepCounterSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.StepCounterSensor")
    Q_PROPERTY(Unsigned steps READ steps)

public:
    StepCounterSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Unsigned steps() const;

Q_SIGNALS:
    void StepCounterChanged(const Unsigned& value);
};

#endif
