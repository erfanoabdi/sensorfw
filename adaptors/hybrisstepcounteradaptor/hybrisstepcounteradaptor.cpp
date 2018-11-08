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
#include "config.h"

#ifndef SENSOR_TYPE_STEP_COUNTER
#define SENSOR_TYPE_STEP_COUNTER (19)
/* If hardware/sensors.h does not define SENSOR_TYPE_STEP_COUNTER, assume
 * a) the event structure is missing u64 union too, and b) u64.step_counter
 * would be located at the same address as non-u64 data array. */
#define NO_SENSORS_EVENT_U64
#endif

HybrisStepCounterAdaptor::HybrisStepCounterAdaptor(const QString& id) :
    HybrisAdaptor(id, SENSOR_TYPE_STEP_COUNTER)
{
    buffer = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("stepcounter", "Internal step counter steps since reboot", buffer);
    setDescription("Hybris step counter");
    powerStatePath = SensorFrameworkConfig::configuration()->value("stepcounter/powerstate_path").toByteArray();
    if (!powerStatePath.isEmpty() && !QFile::exists(powerStatePath))
    {
    	sensordLogW() << "Path does not exists: " << powerStatePath;
    	powerStatePath.clear();
    }
}

HybrisStepCounterAdaptor::~HybrisStepCounterAdaptor()
{
    delete buffer;
}

bool HybrisStepCounterAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;
    if (isRunning() && !powerStatePath.isEmpty())
        writeToFile(powerStatePath, "1");
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
    if (!isRunning() && !powerStatePath.isEmpty())
        writeToFile(powerStatePath, "0");
    sensordLogD() << "Hybris HybrisStepCounterAdaptor stop\n";
}

void HybrisStepCounterAdaptor::processSample(const sensors_event_t& data)
{
    TimedUnsigned *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
#ifdef NO_SENSORS_EVENT_U64
    uint64_t value = 0;
    memcpy(&value, data.data, sizeof value);
    d->value_ = value;
#else
    d->value_ = data.u64.step_counter;
#endif
    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisStepCounterAdaptor::init()
{
}
