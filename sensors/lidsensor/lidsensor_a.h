/**
   @file alssensor_a.h
   @brief D-Bus adaptor for ALSSensor

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

#ifndef LID_SENSOR_H
#define LID_SENSOR_H

#include <QtDBus/QtDBus>
#include <QObject>

#include "datatypes/unsigned.h"
#include "abstractsensor_a.h"

class LidSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(LidSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.lidsensor")
    Q_PROPERTY(Unsigned closed READ closed)

public:
    LidSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Unsigned closed() const;

Q_SIGNALS:
    void LidChanged(bool value);
};

#endif
