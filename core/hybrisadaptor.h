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

#ifndef HybrisAdaptor_H
#define HybrisAdaptor_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QFile>

#include <pthread.h>

#include "deviceadaptor.h"
#include <hardware/sensors.h>
#define SENSORFW_MCE_WATCHER

class HybrisAdaptor;

struct HybrisSensorState
{
    HybrisSensorState();
    ~HybrisSensorState();

    int  m_minDelay;
    int  m_maxDelay;
    int  m_delay;
    int  m_active;
    sensors_event_t m_fallbackEvent;
};

class HybrisManager : public QObject
{
    Q_OBJECT
public:
    static HybrisManager *instance();

    explicit HybrisManager(QObject *parent = 0);
    virtual ~HybrisManager();

    /* - - - - - - - - - - - - - - - - - - - *
     * android sensor hal functions
     * - - - - - - - - - - - - - - - - - - - */

    sensors_event_t *eventForHandle(int handle) const;
    int              indexForHandle(int handle) const;
    int              indexForType  (int sensorType) const;
    int              handleForType (int sensorType) const;
    float            getMaxRange   (int handle) const;
    float            getResolution (int handle) const;
    int              getMinDelay   (int handle) const;
    int              getMaxDelay   (int handle) const;
    int              getDelay      (int handle) const;
    bool             setDelay      (int handle, int delay_ms);
    bool             getActive     (int handle) const;
    bool             setActive     (int handle, bool active);

    /* - - - - - - - - - - - - - - - - - - - *
     * HybrisManager <--> sensorfwd
     * - - - - - - - - - - - - - - - - - - - */

    void startReader     (HybrisAdaptor *adaptor);
    void stopReader      (HybrisAdaptor *adaptor);
    void registerAdaptor (HybrisAdaptor * adaptor);
    void processSample   (const sensors_event_t& data);

private:
    // fields
    bool                          m_initialized;
    QMap <int, HybrisAdaptor *>   m_registeredAdaptors; // type -> obj
    struct sensors_module_t      *m_halModule;
    struct sensors_poll_device_t *m_halDevice;
    int                           m_sensorCount;
    const struct sensor_t        *m_sensorArray;   // [m_sensorCount]
    HybrisSensorState            *m_sensorState;   // [m_sensorCount]
    QMap <int, int>               m_indexOfType;   // type   -> index
    QMap <int, int>               m_indexOfHandle; // handle -> index
    pthread_t                     m_halEventReaderTid;

    friend class HybrisAdaptorReader;

private:
    static void *halEventReaderThread(void *aptr);
};

class HybrisAdaptor : public DeviceAdaptor
{
public:
    HybrisAdaptor(const QString& id, int type);
    virtual ~HybrisAdaptor();

    virtual void init();

    virtual bool startAdaptor();
    bool         isRunning() const;
    virtual void stopAdaptor();

    void         evaluateSensor();
    virtual bool startSensor();
    virtual void stopSensor();

    virtual bool standby();
    virtual bool resume();

    virtual void sendInitialData();

    friend class HybrisManager;

protected:
    virtual void processSample(const sensors_event_t& data) = 0;

    qreal        minRange() const;
    qreal        maxRange() const;
    qreal        resolution() const;

    unsigned int minInterval() const;
    unsigned int maxInterval() const;

    virtual unsigned int interval() const;
    virtual bool setInterval(const unsigned int value, const int sessionId);
    virtual unsigned int evaluateIntervalRequests(int& sessionId) const;
    static bool writeToFile(const QByteArray& path, const QByteArray& content);

private:
    bool          m_inStandbyMode;
    volatile bool m_isRunning;
    bool          m_shouldBeRunning;

    int           m_sensorHandle;
    int           m_sensorType;
};

#endif // HybrisAdaptor_H
