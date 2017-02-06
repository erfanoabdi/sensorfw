/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
** Copyright (C) 2016 kimmoli
** Contact: kimmo.lindholm@eke.fi
**
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hybrisstepcounteradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <hardware/sensors.h>

HybrisStepCounterAdaptor::HybrisStepCounterAdaptor(const QString& id) :
    HybrisAdaptor(id, SENSOR_TYPE_STEP_COUNTER)
{
    buffer = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("stepcounter", "Internal step counter steps since reboot", buffer);
    setDescription("Hybris step counter");
}

HybrisStepCounterAdaptor::~HybrisStepCounterAdaptor()
{
    delete buffer;
}

bool HybrisStepCounterAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;

    sensordLogD() << "Hybris HybrisStepCounterAdaptor start\n";
    return true;
}

void HybrisStepCounterAdaptor::sendInitialData()
{
    sensordLogW() << "No initial data for step counter";
}

void HybrisStepCounterAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "Hybris HybrisStepCounterAdaptor stop\n";
}

void HybrisStepCounterAdaptor::processSample(const sensors_event_t& data)
{
    TimedUnsigned *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
    d->value_ = data.u64.step_counter;
    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisStepCounterAdaptor::init()
{
}
