/**
   @file iioadaptor.h
   @brief IioAdaptor based on SysfsAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2012 Tuomas Kulve
   Copyright (C) 2016 Canonical

   @author Tuomas Kulve <tuomas@kulve.fi>
   @author Lorn Potter <lorn.potter@canonical.com>

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

#ifndef IIOADAPTOR_H
#define IIOADAPTOR_H

#include <sysfsadaptor.h>
#include <datatypes/orientationdata.h>

#define IIO_SYSFS_BASE              "/sys/bus/iio/devices/"


#define IIO_ACCELEROMETER_ENABLE    "buffer/enable"
#define IIO_GYROSCOPE_ENABLE        "buffer/enable"
// FIXME: no enable for magn?
#define IIO_MAGNETOMETER_ENABLE     "compass_cali_test"

// FIXME: shouldn't assume any number of devices
#define IIO_MAX_DEVICES             3

// FIXME: shouldn't assume any number of channels per device
#define IIO_MAX_DEVICE_CHANNELS     20

// FIXME: no idea what would be reasonable length
#define IIO_BUFFER_LEN              256

struct iio_device {
  QString name;
  int channels;
  int channel_bytes[IIO_MAX_DEVICE_CHANNELS];
};

/**
 * @brief Adaptor for Industrial I/O.
 *
 * Adaptor for Industrial I/O. Uses SysFs driver interface in
 * polling mode, i.e. values are read with given constant interval.
 *
 * Driver interface is located in @e /sys/bus/iio/devices/iio:deviceX/ .
 * <ul><li>@e angular_rate filehandle provides measurement values.</li></ul>
 * No other filehandles are currently in use by this adaptor.
 */
class IioAdaptor : public SysfsAdaptor
{
    Q_OBJECT
    enum IioSensorType {
        IIO_ACCELEROMETER = 1, // accel_3d
        IIO_GYROSCOPE, // gyro_3d
        IIO_MAGNETOMETER, // magn_3d
        IIO_ROTATION, // dev_rotation, quaternion
        IIO_ALS, // als
        IIO_TILT // incli_3d
    };

public:
    /**
     * Factory method for gaining a new instance of this adaptor class.
     *
     * @param id Identifier for the adaptor.
     * @return A pointer to created adaptor with base class type.
     */
    static DeviceAdaptor *factoryMethod(const QString& id)
    {
        return new IioAdaptor(id);
    }

    virtual bool startSensor();
    virtual void stopSensor();
//    virtual bool standby();
//    virtual bool resume();

protected:

    /**
     * Constructor. Protected to force externals to use factory method.
     *
     * @param id Identifier for the adaptor.
     */
    IioAdaptor(const QString &id/*, int type*/);

    /**
     * Destructor.
     */
    ~IioAdaptor();


    bool setInterval(const unsigned int value, const int sessionId);
  //  unsigned int interval() const;

private:

    /**
     * Read and process data. Run when sysfsadaptor has detected new
     * available data.
     *
     * @param pathId PathId for the file that had event.
     * @param fd Open file descriptor with new data. See
     *           #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    int sensorExists(IioAdaptor::IioSensorType sensor);
    int findSensor(const QString &name);
	bool deviceEnable(int device, int enable);

    QString deviceGetName(int device);
	bool sysfsWriteInt(QString filename, int val);
	QString sysfsReadString(QString filename);
	int sysfsReadInt(QString filename);
	int scanElementsEnable(int device, int enable);
	int deviceChannelParseBytes(QString filename);

	// Device number for the sensor (-1 if not found)
    int dev_accl_;

    DeviceAdaptorRingBuffer<TimedXyzData>* iioXyzBuffer_;
    DeviceAdaptorRingBuffer<TimedUnsigned>* alsBuffer_;
    DeviceAdaptorRingBuffer<CalibratedMagneticFieldData>* magnetometerBuffer_;

	struct iio_device devices_[IIO_MAX_DEVICES];
    QString deviceId;
    IioSensorType sensorType;
    QString devicePath;
    double scale;
    int frequency;
    int offset;
    int numChannels;

    TimedXyzData* timedData;
    CalibratedMagneticFieldData *calData;
    TimedUnsigned *uData;

private slots:
    void setup();
};

#endif
