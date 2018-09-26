/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
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

#include "hybrisadaptor.h"
#include "deviceadaptor.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#ifndef SENSOR_TYPE_ACCELEROMETER
#define SENSOR_TYPE_ACCELEROMETER (1)
#endif
#ifndef SENSOR_TYPE_MAGNETIC_FIELD
#define SENSOR_TYPE_MAGNETIC_FIELD (2)
#endif
#ifndef SENSOR_TYPE_ORIENTATION
#define SENSOR_TYPE_ORIENTATION (3)
#endif
#ifndef SENSOR_TYPE_GYROSCOPE
#define SENSOR_TYPE_GYROSCOPE (4)
#endif
#ifndef SENSOR_TYPE_LIGHT
#define SENSOR_TYPE_LIGHT (5)
#endif
#ifndef SENSOR_TYPE_PRESSURE
#define SENSOR_TYPE_PRESSURE (6)
#endif
#ifndef SENSOR_TYPE_TEMPERATURE
#define SENSOR_TYPE_TEMPERATURE (7)
#endif
#ifndef SENSOR_TYPE_PROXIMITY
#define SENSOR_TYPE_PROXIMITY (8)
#endif
#ifndef SENSOR_TYPE_GRAVITY
#define SENSOR_TYPE_GRAVITY (9)
#endif
#ifndef SENSOR_TYPE_LINEAR_ACCELERATION
#define SENSOR_TYPE_LINEAR_ACCELERATION (10)
#endif
#ifndef SENSOR_TYPE_ROTATION_VECTOR
#define SENSOR_TYPE_ROTATION_VECTOR (11)
#endif
#ifndef SENSOR_TYPE_RELATIVE_HUMIDITY
#define SENSOR_TYPE_RELATIVE_HUMIDITY (12)
#endif
#ifndef SENSOR_TYPE_AMBIENT_TEMPERATURE
#define SENSOR_TYPE_AMBIENT_TEMPERATURE (13)
#endif
//#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED (14)
//#define SENSOR_TYPE_GAME_ROTATION_VECTOR (15)
//#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED (16)
//#define SENSOR_TYPE_SIGNIFICANT_MOTION (17)
//#define SENSOR_TYPE_STEP_DETECTOR (18)
#ifndef SENSOR_TYPE_STEP_COUNTER
#define SENSOR_TYPE_STEP_COUNTER (19)
#endif
//#define SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR (20)

/* ========================================================================= *
 * UTILITIES
 * ========================================================================= */

static char const *
sensorTypeName(int type)
{
    switch(type) {
    case SENSOR_TYPE_META_DATA:                   return "META_DATA";
    case SENSOR_TYPE_ACCELEROMETER:               return "ACCELEROMETER";
    case SENSOR_TYPE_GEOMAGNETIC_FIELD:           return "GEOMAGNETIC_FIELD";
    case SENSOR_TYPE_ORIENTATION:                 return "ORIENTATION";
    case SENSOR_TYPE_GYROSCOPE:                   return "GYROSCOPE";
    case SENSOR_TYPE_LIGHT:                       return "LIGHT";
    case SENSOR_TYPE_PRESSURE:                    return "PRESSURE";
    case SENSOR_TYPE_TEMPERATURE:                 return "TEMPERATURE";
    case SENSOR_TYPE_PROXIMITY:                   return "PROXIMITY";
    case SENSOR_TYPE_GRAVITY:                     return "GRAVITY";
    case SENSOR_TYPE_LINEAR_ACCELERATION:         return "LINEAR_ACCELERATION";
    case SENSOR_TYPE_ROTATION_VECTOR:             return "ROTATION_VECTOR";
    case SENSOR_TYPE_RELATIVE_HUMIDITY:           return "RELATIVE_HUMIDITY";
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:         return "AMBIENT_TEMPERATURE";
    case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED: return "MAGNETIC_FIELD_UNCALIBRATED";
    case SENSOR_TYPE_GAME_ROTATION_VECTOR:        return "GAME_ROTATION_VECTOR";
    case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:      return "GYROSCOPE_UNCALIBRATED";
    case SENSOR_TYPE_SIGNIFICANT_MOTION:          return "SIGNIFICANT_MOTION";
    case SENSOR_TYPE_STEP_DETECTOR:               return "STEP_DETECTOR";
    case SENSOR_TYPE_STEP_COUNTER:                return "STEP_COUNTER";
    case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR: return "GEOMAGNETIC_ROTATION_VECTOR";
    case SENSOR_TYPE_HEART_RATE:                  return "HEART_RATE";
    case SENSOR_TYPE_TILT_DETECTOR:               return "TILT_DETECTOR";
    case SENSOR_TYPE_WAKE_GESTURE:                return "WAKE_GESTURE";
    case SENSOR_TYPE_GLANCE_GESTURE:              return "GLANCE_GESTURE";
    case SENSOR_TYPE_PICK_UP_GESTURE:             return "PICK_UP_GESTURE";
    case SENSOR_TYPE_WRIST_TILT_GESTURE:          return "WRIST_TILT_GESTURE";
    }

    static char buf[32];
    snprintf(buf, sizeof buf, "type%d", type);
    return buf;
}

static void ObtainTemporaryWakeLock()
{
    static bool triedToOpen = false;
    static int wakeLockFd = -1;

    if (!triedToOpen) {
        triedToOpen = true;
        wakeLockFd = ::open("/sys/power/wake_lock", O_RDWR);
        if (wakeLockFd == -1) {
            sensordLogW() << "wake locks not available:" << ::strerror(errno);
        }
    }

    if (wakeLockFd != -1) {
        sensordLogD() << "wake lock to guard sensor data io";
        static const char m[] = "sensorfwd_pass_data 1000000000\n";
        if (::write(wakeLockFd, m, sizeof m - 1) == -1) {
            sensordLogW() << "wake locking failed:" << ::strerror(errno);
            ::close(wakeLockFd), wakeLockFd = -1;
        }
    }
}

/* ========================================================================= *
 * HybrisSensorState
 * ========================================================================= */

HybrisSensorState::HybrisSensorState()
    : m_minDelay(0)
    , m_maxDelay(0)
    , m_delay(-1)
    , m_active(-1)
{
}

HybrisSensorState::~HybrisSensorState()
{
}

/* ========================================================================= *
 * HybrisManager
 * ========================================================================= */

Q_GLOBAL_STATIC(HybrisManager, hybrisManager)

HybrisManager::HybrisManager(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
    , m_registeredAdaptors()
    , m_halModule(NULL)
    , m_halDevice(NULL)
    , m_halSensorCount(0)
    , m_halSensorArray(NULL)
    , m_halSensorState(NULL)
    , m_halIndexOfType()
    , m_halIndexOfHandle()
    , m_halEventReaderTid(0)
{
    int err;

    /* Open android sensor plugin */
    err = hw_get_module(SENSORS_HARDWARE_MODULE_ID,
                        (hw_module_t const**)&m_halModule);
    if (err != 0) {
        m_halModule = 0;
        sensordLogW() << "hw_get_module() failed" <<  strerror(-err);
        return ;
    }

    /* Open android sensor device */
    err = sensors_open(&m_halModule->common, &m_halDevice);
    if (err != 0) {
        m_halDevice = 0;
        sensordLogW() << "sensors_open() failed:" << strerror(-err);
        return;
    }

    /* Get static sensor information */
    m_halSensorCount = m_halModule->get_sensors_list(m_halModule, &m_halSensorArray);

    /* Reserve space for sensor state data */
    m_halSensorState = new HybrisSensorState[m_halSensorCount];

    /* Select and initialize sensors to be used */
    for (int i = 0 ; i < m_halSensorCount ; i++) {
        /* Always do handle -> index mapping */
        m_halIndexOfHandle.insert(m_halSensorArray[i].handle, i);

        bool use = true;
        // Assumption: The primary sensor variants that we want to
        // use are listed before the secondary ones that we want
        // to ignore -> Use the 1st entry found for each sensor type.
        if (m_halIndexOfType.contains(m_halSensorArray[i].type)) {
            use = false;
        }

        // some devices have compass and compass raw,
        // ignore compass raw. compass has range 360
        if (m_halSensorArray[i].type == SENSOR_TYPE_ORIENTATION &&
            m_halSensorArray[i].maxRange != 360) {
            use = false;
        }

        sensordLogD() << Q_FUNC_INFO
            << (use ? "SELECT" : "IGNORE")
            << "type:" << m_halSensorArray[i].type
            << "name:" << (m_halSensorArray[i].name ?: "n/a");

        if (use) {
            // min/max delay is specified in [us] -> convert to [ms]
            int minDelay = (m_halSensorArray[i].minDelay + 999) / 1000;
            int maxDelay = (m_halSensorArray[i].maxDelay + 999) / 1000;

            // Positive minDelay means delay /can/ be set - but depending
            // on sensor hal implementation it can also mean that some
            // delay /must/ be set or the sensor does not start reporting
            // despite being enabled -> as an protection agains clients
            // failing to explicitly set delays / using delays that would
            // get rejected by upper levels of sensorfwd logic -> setup
            // 200 ms delay (capped to reported min/max range).
            if (minDelay >= 0) {
                if (maxDelay < minDelay)
                    maxDelay = minDelay;

                int delay = minDelay ? 200 : 0;
                if (delay < minDelay)
                    delay = minDelay;
                else if (delay > maxDelay )
                    delay = maxDelay;

                m_halSensorState[i].m_minDelay = minDelay;
                m_halSensorState[i].m_maxDelay = maxDelay;

                halSetActive(m_halSensorArray[i].handle, true);
                halSetDelay(m_halSensorArray[i].handle, delay);
            }
            m_halIndexOfType.insert(m_halSensorArray[i].type, i);

        /* Make sure all sensors are initially in stopped state */
        halSetActive(m_halSensorArray[i].handle, false);
    }

    /* Start android sensor event reader */
    err = pthread_create(&m_halEventReaderTid, 0, halEventReaderThread, this);
    if (err) {
        m_halEventReaderTid = 0;
        sensordLogC() << "Failed to start hal reader thread";
        return;
    }
    sensordLogD() << "Hal reader thread started";

    m_initialized = true;
}

HybrisManager::~HybrisManager()
{
    sensordLogD() << "stop all sensors";
    foreach (HybrisAdaptor *adaptor, m_registeredAdaptors.values()) {
        adaptor->stopSensor();
    }

    if (m_halDevice) {
        sensordLogD() << "close sensor device";
        int errorCode = sensors_close(m_halDevice);
        if (errorCode != 0) {
            sensordLogW() << "sensors_close() failed:" << strerror(-errorCode);
        }
        m_halDevice = NULL;
    }

    if (m_halEventReaderTid) {
        sensordLogD() << "Canceling hal reader thread";
        int err = pthread_cancel(m_halEventReaderTid);
        if (err) {
            sensordLogC() << "Failed to cancel hal reader thread";
        } else {
            sensordLogD() << "Waiting for hal reader thread to exit";
            void *ret = 0;
            struct timespec tmo = { 0, 0};
            clock_gettime(CLOCK_REALTIME, &tmo);
            tmo.tv_sec += 3;
            err = pthread_timedjoin_np(m_halEventReaderTid, &ret, &tmo);
            if (err) {
                sensordLogC() << "Hal reader thread did not exit";
            } else {
                sensordLogD() << "Hal reader thread terminated";
                m_halEventReaderTid = 0;
            }
        }
        if (m_halEventReaderTid) {
            /* The reader thread is stuck at android hal blob.
             * Continuing would be likely to release resourse
             * still in active use and lead to segfaulting.
             * Resort to doing a quick and dirty exit. */
            _exit(EXIT_FAILURE);
        }
    }
    delete[] m_halSensorState;
}

HybrisManager *HybrisManager::instance()
{
    HybrisManager *priv = hybrisManager();
    return priv;
}

int HybrisManager::halHandleForType(int sensorType) const
{
    int index = halIndexForType(sensorType);
    return (index < 0) ? -1 : m_halSensorArray[index].handle;
}

int HybrisManager::halIndexForHandle(int handle) const
{
    int index = m_halIndexOfHandle.value(handle, -1);
    if (index == -1)
        sensordLogW("HYBRIS CTL invalid sensor handle: %d", handle);
    return index;
}

int HybrisManager::halIndexForType(int sensorType) const
{
    int index = m_halIndexOfType.value(sensorType, -1);
    if (index == -1)
        sensordLogW("HYBRIS CTL invalid sensor type: %d", sensorType);
    return index;
}

void HybrisManager::startReader(HybrisAdaptor *adaptor)
{
    if (m_registeredAdaptors.values().contains(adaptor)) {
        sensordLogD() << "activating " << adaptor->name() << adaptor->m_sensorHandle;
        if (!halSetActive(adaptor->m_sensorHandle, true)) {
            sensordLogW() <<Q_FUNC_INFO<< "failed";
            adaptor->setValid(false);
        }
    }
}

void HybrisManager::stopReader(HybrisAdaptor *adaptor)
{
    if (m_registeredAdaptors.values().contains(adaptor)) {
            sensordLogD() << "deactivating " << adaptor->name();
            if (!halSetActive(adaptor->m_sensorHandle, false)) {
                sensordLogW() <<Q_FUNC_INFO<< "failed";
            }
    }
}

void HybrisManager::processSample(const sensors_event_t& data)
{
    foreach (HybrisAdaptor *adaptor, m_registeredAdaptors.values(data.type)) {
        if (adaptor->isRunning()) {
            adaptor->processSample(data);
        }
    }
}

void HybrisManager::registerAdaptor(HybrisAdaptor *adaptor)
{
    if (!m_registeredAdaptors.values().contains(adaptor) && adaptor->isValid()) {
        m_registeredAdaptors.insertMulti(adaptor->m_sensorType, adaptor);
    }
}

float HybrisManager::halGetMaxRange(int handle) const
{
    float range = 0;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];

        range = sensor->maxRange;
        sensordLogT("HYBRIS CTL getMaxRange(%d=%s) -> %g",
                    sensor->handle, sensorTypeName(sensor->type), range);
    }

    return range;
}

float HybrisManager::halGetResolution(int handle) const
{
    float resolution = 0;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];

        resolution = sensor->resolution;
        sensordLogT("HYBRIS CTL getResolution(%d=%s) -> %g",
                    sensor->handle, sensorTypeName(sensor->type), resolution);
    }

    return resolution;
}

int HybrisManager::halGetMinDelay(int handle) const
{
    int delay = 0;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];
        HybrisSensorState     *state  = &m_halSensorState[index];

        delay = state->m_minDelay;
        sensordLogT("HYBRIS CTL getMinDelay(%d=%s) -> %d",
                    sensor->handle, sensorTypeName(sensor->type), delay);
    }

    return delay;
}

int HybrisManager::halGetMaxDelay(int handle) const
{
    int delay = 0;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];
        HybrisSensorState     *state  = &m_halSensorState[index];

        delay = state->m_maxDelay;
        sensordLogT("HYBRIS CTL getMaxDelay(%d=%s) -> %d",
                    sensor->handle, sensorTypeName(sensor->type), delay);
    }

    return delay;
}

int HybrisManager::halGetDelay(int handle) const
{
    int delay = 0;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];
        HybrisSensorState     *state  = &m_halSensorState[index];

        delay = state->m_delay;
        sensordLogT("HYBRIS CTL getDelay(%d=%s) -> %d",
                    sensor->handle, sensorTypeName(sensor->type), delay);
    }

    return delay;
}

bool HybrisManager::halSetDelay(int handle, int delay_ms)
{
    bool success = false;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];
        HybrisSensorState     *state  = &m_halSensorState[index];

        if (state->m_delay == delay_ms) {
            sensordLogT("HYBRIS CTL setDelay(%d=%s, %d) -> no-change",
                        sensor->handle, sensorTypeName(sensor->type), delay_ms);
        } else {
            int64_t delay_ns = delay_ms * 1000LL * 1000LL;
            int error = m_halDevice->setDelay(m_halDevice, sensor->handle, delay_ns);
            if (error) {
                sensordLogW("HYBRIS CTL setDelay(%d=%s, %d) -> %d=%s",
                            sensor->handle, sensorTypeName(sensor->type), delay_ms,
                            error, strerror(error));
            } else {
                sensordLogD("HYBRIS CTL setDelay(%d=%s, %d) -> success",
                            sensor->handle, sensorTypeName(sensor->type), delay_ms);
                state->m_delay = delay_ms;
                success = true;
            }
        }
    }

    return success;
}

bool HybrisManager::halGetActive(int handle) const
{
    bool active = false;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];
        HybrisSensorState     *state  = &m_halSensorState[index];

        active = (state->m_active > 0);
        sensordLogT("HYBRIS CTL getActive(%d=%s) -> %s",
                    sensor->handle, sensorTypeName(sensor->type),
                    active ? "true" : "false");
    }
    return active;
}

bool HybrisManager::halSetActive(int handle, bool active)
{
    bool success = false;
    int index = halIndexForHandle(handle);

    if (index != -1) {
        const struct sensor_t *sensor = &m_halSensorArray[index];
        HybrisSensorState     *state  = &m_halSensorState[index];

        if (state->m_active == active) {
            sensordLogT("HYBRIS CTL setActive%d=%s, %s) -> no-change",
                        sensor->handle, sensorTypeName(sensor->type), active ? "true" : "false");
            success = true;
        } else {
            int error = m_halDevice->activate(m_halDevice, sensor->handle, active);
            if (error) {
                sensordLogW("HYBRIS CTL setActive%d=%s, %s) -> %d=%s",
                            sensor->handle, sensorTypeName(sensor->type), active ? "true" : "false",
                            error, strerror(error));
            } else {
                sensordLogD("HYBRIS CTL setActive%d=%s, %s) -> success",
                            sensor->handle, sensorTypeName(sensor->type), active ? "true" : "false");
                state->m_active = active;
                success = true;
            }
            if (state->m_active == true && state->m_delay != -1) {
                sensordLogD("HYBRIS CTL FORCE DELAY UPDATE");
                int delay_ms = state->m_delay;
                state->m_delay = -1;
                halSetDelay(handle, delay_ms);
            }
        }
    }
    return success;
}

void *HybrisManager::halEventReaderThread(void *aptr)
{
    HybrisManager *manager = static_cast<HybrisManager *>(aptr);
    static const size_t numEvents = 16;
    sensors_event_t buffer[numEvents];
    /* Async cancellation, but disabled */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    /* Leave INT/TERM signal processing up to the main thread */
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, SIGINT);
    sigaddset(&ss, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &ss, 0);
    /* Loop until explicitly canceled */
    for( ;; ) {
        /* Async cancellation point at android hal poll() */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
        int numberOfEvents = manager->m_halDevice->poll(manager->m_halDevice, buffer, numEvents);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
        /* Rate limit in poll() error situations */
        if (numberOfEvents < 0) {
            sensordLogW() << "android device->poll() failed" << strerror(-numberOfEvents);
            struct timespec ts = { 1, 0 }; // 1000 ms
            do { } while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
            continue;
        }
        /* Process received events */
        bool blockSuspend = false;
        bool errorInInput = false;
        for (int i = 0; i < numberOfEvents; i++) {
            const sensors_event_t& data = buffer[i];

            sensordLogT("HYBRIS EVE %s", sensorTypeName(data.type));

            if (data.version != sizeof(sensors_event_t)) {
                sensordLogW()<< QString("incorrect event version (version=%1, expected=%2").arg(data.version).arg(sizeof(sensors_event_t));
                errorInInput = true;
            }
            if (data.type == SENSOR_TYPE_PROXIMITY) {
                blockSuspend = true;
            }
            // FIXME: is this thread safe?
            manager->processSample(data);
        }
        /* Suspend proof sensor reporting that could occur in display off */
        if (blockSuspend) {
            ObtainTemporaryWakeLock();
        }
        /* Rate limit after receiving erraneous events */
        if (errorInInput) {
            struct timespec ts = { 0, 50 * 1000 * 1000 }; // 50 ms
            do { } while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
        }
    }
    return 0;
}

/* ========================================================================= *
 * HybrisAdaptor
 * ========================================================================= */

HybrisAdaptor::HybrisAdaptor(const QString& id, int type)
    : DeviceAdaptor(id)
    , m_inStandbyMode(false)
    , m_isRunning(false)
    , m_shouldBeRunning(false)
    , m_sensorHandle(-1)
    , m_sensorType(type)
{
    m_sensorHandle = hybrisManager()->halHandleForType(m_sensorType);
    if (m_sensorHandle == -1) {
        sensordLogW() << Q_FUNC_INFO <<"no such sensor" << id;
        setValid(false);
        return;
    }

    hybrisManager()->registerAdaptor(this);
}

HybrisAdaptor::~HybrisAdaptor()
{
}

void HybrisAdaptor::init()
{
    introduceAvailableDataRange(DataRange(minRange(), maxRange(), resolution()));
    introduceAvailableInterval(DataRange(minInterval(), maxInterval(), 0));
}

void HybrisAdaptor::sendInitialData()
{
    // virtual dummy
    // used for ps/als initial value hacks
}

bool HybrisAdaptor::writeToFile(const QByteArray& path, const QByteArray& content)
{
    sensordLogT() << "Writing to '" << path << ": " << content;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        sensordLogW() << "Failed to open '" << path << "': " << file.errorString();
        return false;
    }
    if (file.write(content.constData(), content.size()) == -1)
    {
        sensordLogW() << "Failed to write to '" << path << "': " << file.errorString();
        file.close();
        return false;
    }

    file.close();
    return true;
}

/* ------------------------------------------------------------------------- *
 * range
 * ------------------------------------------------------------------------- */

qreal HybrisAdaptor::minRange() const
{
    return 0;
}

qreal HybrisAdaptor::maxRange() const
{
    return hybrisManager()->halGetMaxRange(m_sensorHandle);
}

qreal HybrisAdaptor::resolution() const
{
    return hybrisManager()->halGetResolution(m_sensorHandle);
}

/* ------------------------------------------------------------------------- *
 * interval
 * ------------------------------------------------------------------------- */

unsigned int HybrisAdaptor::minInterval() const
{
    return hybrisManager()->halGetMinDelay(m_sensorHandle);
}

unsigned int HybrisAdaptor::maxInterval() const
{
    return hybrisManager()->halGetMaxDelay(m_sensorHandle);
}

unsigned int HybrisAdaptor::interval() const
{
    return hybrisManager()->halGetDelay(m_sensorHandle);
}

bool HybrisAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    Q_UNUSED(sessionId);

    bool ok = hybrisManager()->halSetDelay(m_sensorHandle, value);

    if (!ok) {
        sensordLogW() << Q_FUNC_INFO << "setInterval not ok";
    } else {
        sendInitialData();
    }

    return ok;
}

unsigned int HybrisAdaptor::evaluateIntervalRequests(int& sessionId) const
{
    if (m_intervalMap.size() == 0)
    {
        sessionId = -1;
        return defaultInterval();
    }

    // Get the smallest positive request, 0 is reserved for HW wakeup
    QMap<int, unsigned int>::const_iterator it = m_intervalMap.constBegin();
    unsigned int highestValue = it.value();
    int winningSessionId = it.key();

    for (++it; it != m_intervalMap.constEnd(); ++it)
    {
        if (((it.value() < highestValue) && (it.value() > 0)) || highestValue == 0) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue > 0 ? highestValue : defaultInterval();
}

/* ------------------------------------------------------------------------- *
 * start/stop adaptor
 * ------------------------------------------------------------------------- */

bool HybrisAdaptor::startAdaptor()
{
    return isValid();
}

void HybrisAdaptor::stopAdaptor()
{
    if (getAdaptedSensor()->isRunning())
        stopSensor();
}

/* ------------------------------------------------------------------------- *
 * start/stop sensor
 * ------------------------------------------------------------------------- */

bool HybrisAdaptor::isRunning() const
{
    return m_isRunning;
}

void HybrisAdaptor::evaluateSensor()
{
    // Get listener object
    AdaptedSensorEntry *entry = getAdaptedSensor();
    if (entry == NULL) {
        sensordLogW() << Q_FUNC_INFO << "Sensor not found: " << name();
        return;
    }

    // Check policy
    bool runningAllowed = (deviceStandbyOverride() || !m_inStandbyMode);

    // Target state
    bool startRunning = m_shouldBeRunning && runningAllowed;

    if (m_isRunning != startRunning) {
        if ((m_isRunning = startRunning)) {
            hybrisManager()->startReader(this);
            if (entry->addReference() == 1) {
                entry->setIsRunning(true);
            }
        } else {
            if (entry->removeReference() == 0) {
                entry->setIsRunning(false);
            }
            hybrisManager()->stopReader(this);
        }
        sensordLogT() << Q_FUNC_INFO << "entry" << entry->name()
                      << "refs:" << entry->referenceCount() << "running:" << entry->isRunning();
    }
}

bool HybrisAdaptor::startSensor()
{
    // Note: This is overloaded and called by each HybrisXxxAdaptor::startSensor()
    if (!m_shouldBeRunning) {
        m_shouldBeRunning = true;
        sensordLogT("%s m_shouldBeRunning = %d", sensorTypeName(m_sensorType), m_shouldBeRunning);
        evaluateSensor();
    }
    return true;
}

void HybrisAdaptor::stopSensor()
{
    // Note: This is overloaded and called by each HybrisXxxAdaptor::stopSensor()
    if (m_shouldBeRunning) {
        m_shouldBeRunning = false;
        sensordLogT("%s m_shouldBeRunning = %d", sensorTypeName(m_sensorType), m_shouldBeRunning);
        evaluateSensor();
    }
}

bool HybrisAdaptor::standby()
{
    if (!m_inStandbyMode) {
        m_inStandbyMode = true;
        sensordLogT("%s m_inStandbyMode = %d", sensorTypeName(m_sensorType), m_inStandbyMode);
        evaluateSensor();
    }
    return true;
}

bool HybrisAdaptor::resume()
{
    if (m_inStandbyMode) {
        m_inStandbyMode = false;
        sensordLogT("%s m_inStandbyMode = %d", sensorTypeName(m_sensorType), m_inStandbyMode);
        evaluateSensor();
    }
    return true;
}
