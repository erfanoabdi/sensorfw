/**

//    http://www.apache.org/licenses/LICENSE-2.0

   @file iioadaptor.cpp
   @brief IioAdaptor based on SysfsAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2012 Tuomas Kulve
   Copyright (C) 2012 Srdjan Markovic
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
#include <errno.h>

#include <iio.h>
#include <libudev.h>

#include <logging.h>
#include <config.h>
#include <datatypes/utils.h>
#include <unistd.h>
#include <time.h>

#include "iioadaptor.h"
#include <sensord-qt5/sysfsadaptor.h>
#include <sensord-qt5/deviceadaptorringbuffer.h>
#include <QTextStream>
#include <QDir>
#include <QTimer>
#include <QDirIterator>
#include <qmath.h>

#include <sensord-qt5/deviceadaptor.h>
#include "datatypes/orientationdata.h"
#define MAX_TYPE_SPEC_LEN   32

typedef struct
{
        char sign;
        char endianness;
        short realbits;
        short storagebits;
        short shift;
}
datum_info_t;

typedef struct
{
        int offset;     /* Offset in bytes within the iio character device report */
        int size;       /* Field size in bytes */
        float scale;    /* Scale for each channel */
        char type_spec[MAX_TYPE_SPEC_LEN];      /* From driver; ex: le:u10/16>>0 */
        datum_info_t type_info;                 /* Decoded contents of type spec */
        float opt_scale; /*
                          * Optional correction scale read from a property such as iio.accel.x.scale, allowing late compensation of
                          * problems such as misconfigured axes ; set to 1 by default. Applied at the end of the scaling process.
                          */
        int raw_path_present;   /* Flag signalling the presence of in_<sens>_<axis>_raw file */
        int input_path_present; /* Flag signalling the presence of in_<sens>_input file */
}
channel_info_t;


#define GRAVITY         9.80665
#define REV_GRAVITY     0.101936799

/* Conversion of acceleration data to SI units (m/s^2) */
#define CONVERT_A_X(x)  ((float(x) / 1000) * (GRAVITY * -1.0))
#define CONVERT_A_Y(x)  ((float(x) / 1000) * (GRAVITY * 1.0))
#define CONVERT_A_Z(x)  ((float(x) / 1000) * (GRAVITY * 1.0))

IioAdaptor::IioAdaptor(const QString &id/*, int type*/) :
        SysfsAdaptor(id, SysfsAdaptor::IntervalMode, true),
        deviceId(id),
        scale(-1)
{
    sensordLogD() << "Creating IioAdaptor with id: " << id;
    setup();
}

IioAdaptor::~IioAdaptor()
{
    if (iioXyzBuffer_)
        delete iioXyzBuffer_;
    if (alsBuffer_)
        delete alsBuffer_;
    if (magnetometerBuffer_)
        delete magnetometerBuffer_;
}

void IioAdaptor::setup()
{
    if (deviceId.startsWith("accel")) {
        dev_accl_ = sensorExists(IioAdaptor::IIO_ACCELEROMETER);
        if (dev_accl_!= -1) {
            sensorType = IioAdaptor::IIO_ACCELEROMETER;
            iioXyzBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(1);
            QString desc = "Industrial I/O accelerometer (" +  devices_[dev_accl_].name +")";
            qDebug() << Q_FUNC_INFO << "Accelerometer found";
            setAdaptedSensor("accelerometer", desc, iioXyzBuffer_);
            setDescription(desc);
        }
    }
    else if (deviceId.startsWith("gyro")) {
        dev_accl_ = sensorExists(IIO_GYROSCOPE);
        if (dev_accl_!= -1) {
            sensorType = IioAdaptor::IIO_GYROSCOPE;
            iioXyzBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(1);
            QString desc = "Industrial I/O gyroscope (" +  devices_[dev_accl_].name +")";
            setAdaptedSensor("gyroscope", desc, iioXyzBuffer_);
            setDescription(desc);
        }
    }
    else if (deviceId.startsWith("mag")) {
        dev_accl_ = sensorExists(IIO_MAGNETOMETER);
        if (dev_accl_!= -1) {
            sensorType = IioAdaptor::IIO_MAGNETOMETER;
            magnetometerBuffer_ = new DeviceAdaptorRingBuffer<CalibratedMagneticFieldData>(1);
            QString desc = "Industrial I/O magnetometer (" +  devices_[dev_accl_].name +")";
            setAdaptedSensor("magnetometer", desc, magnetometerBuffer_);
            //        overflowLimit_ = Config::configuration()->value<int>("magnetometer/overflow_limit", 8000);
            setDescription(desc);
        }
    }
    else if (deviceId.startsWith("als")) {
        dev_accl_ = sensorExists(IIO_ALS);
        if (dev_accl_!= -1) {
            sensorType = IioAdaptor::IIO_ALS;
            alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
            QString desc = "Industrial I/O light sensor (" +  devices_[dev_accl_].name +")";
            setDescription(desc);
            setAdaptedSensor("als", desc, alsBuffer_);
        }
    }

    devices_[dev_accl_].channels = scanElementsEnable(dev_accl_,1);
    scanElementsEnable(dev_accl_,0);

    introduceAvailableDataRange(DataRange(0, 65535, 1));
    introduceAvailableInterval(DataRange(0, 586, 0));
    setDefaultInterval(10);
}

// accel_3d
int IioAdaptor::findSensor(const QString &sensorName)
{
    udev_list_entry *devices;
    udev_list_entry *dev_list_entry;
    udev_device *dev;
    struct udev *udevice = 0;
    struct udev_enumerate *enumerate = 0;

    if (!udevice)
        udevice = udev_new();

    enumerate = udev_enumerate_new(udevice);
    udev_enumerate_add_match_subsystem(enumerate, "iio");

    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    int index = 0;
    bool ok2;

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);

        dev = udev_device_new_from_syspath(udevice, path);
        if (qstrcmp(udev_device_get_subsystem(dev), "iio") == 0) {
            QString name = QString::fromLatin1(udev_device_get_sysattr_value(dev,"name"));

            if (name == sensorName) {
                struct udev_list_entry *sysattr;
                int j = 0;
                QString eventName = QString::fromLatin1(udev_device_get_sysname(dev));
                devicePath = QString::fromLatin1(udev_device_get_syspath(dev)) +"/";
                index = eventName.right(1).toInt(&ok2);
                qDebug() << Q_FUNC_INFO << "syspath" << devicePath;

                udev_list_entry_foreach(sysattr, udev_device_get_sysattr_list_entry(dev)) {
                    const char *name;
                    const char *value;
                    bool ok;
                    name = udev_list_entry_get_name(sysattr);
                    value = udev_device_get_sysattr_value(dev, name);
                    if (value == NULL)
                        continue;
                    qDebug() << "attr" << name << value;

                    QString attributeName(name);
                    if (attributeName.endsWith("scale")) {
                        double num = QString(value).toDouble(&ok);
                        if (ok) {
                            scale = num;
                            qDebug() << "scale is" << scale;
                        }
                    } else if (attributeName.endsWith("offset")) {
                        double num = QString(value).toDouble(&ok);
                        if (ok)
                            offset = num;
                        qDebug() << "offset is" << value;
                    } else if (attributeName.endsWith("frequency")) {
                        double num = QString(value).toDouble(&ok);
                        if (ok)
                            frequency = num;
                        qDebug() << "frequency is" << value;
                    } else if (attributeName.endsWith("raw")) {
                        qDebug() << "adding to paths:" << devicePath
                                   << attributeName << index;

                        addPath(devicePath + attributeName, j);
                        j++;
                    }
                }

    // in_rot_from_north_magnetic_tilt_comp_raw ?

                // type
                break;
            }
        }
        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);

    if (ok2)
        return index;
    else
        return -1;
}
/*
 * als
 * accel_3d
 * gyro_3d
 * magn_3d
 * incli_3d
 * dev_rotation
 *
 * */
int IioAdaptor::sensorExists(IioAdaptor::IioSensorType sensor)
{
    QString sensorName;
    switch (sensor) {
    case IIO_ACCELEROMETER:
        sensorName = QStringLiteral("accel_3d");
        break;
    case IIO_GYROSCOPE:
        sensorName = QStringLiteral("gyro_3d");
        break;
    case IIO_MAGNETOMETER:
        sensorName = QStringLiteral("magn_3d");
        break;
    case IIO_ALS:
        sensorName = QStringLiteral("als");
        break;
    case IIO_ROTATION:
        sensorName = QStringLiteral("dev_rotation");
        break;
    case IIO_TILT:
        sensorName = QStringLiteral("incli_3d");
        break;
    default:
        return -1;
    }
    if (!sensorName.isEmpty())
        return findSensor(sensorName);
    else
        return -1;
}

bool IioAdaptor::deviceEnable(int device, int enable)
{
    qDebug() << Q_FUNC_INFO <<"device"<< device <<"enable" << enable;
    qDebug() << "devicePath" << devicePath << devices_[device].name;
    qDebug() << "dev_accl_" << dev_accl_;

    QString pathEnable = devicePath + "buffer/enable";
    QString pathLength = devicePath + "buffer/length";

    qDebug() << pathEnable << pathLength;

    if (enable == 1) {
        // FIXME: should enable sensors for this device? Assuming enabled already
        devices_[device].name = deviceGetName(device);
        numChannels = scanElementsEnable(device, enable);
        devices_[device].channels = numChannels;
        sysfsWriteInt(pathLength, IIO_BUFFER_LEN);
        sysfsWriteInt(pathEnable, enable);
    } else {
        sysfsWriteInt(pathEnable, enable);
        scanElementsEnable(device, enable);
        // FIXME: should disable sensors for this device?
    }

    return true;
}

QString IioAdaptor::deviceGetName(int /*device*/)
{
    QString pathDeviceName = devicePath + "name";
    return sysfsReadString(pathDeviceName);
}

bool IioAdaptor::sysfsWriteInt(QString filename, int val)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        sensordLogW() << "Failed to open " << filename;
        return false;
    }

    QTextStream out(&file);
    out << val << "\n";

    file.close();

	return true;
}

QString IioAdaptor::sysfsReadString(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        sensordLogW() << "Failed to open " << filename;
        return QString();
    }

    QTextStream in(&file);
    QString line = in.readLine();

    if (line.endsWith("\n")) {
        line.chop(1);
    }

    file.close();
    return line;
}

int IioAdaptor::sysfsReadInt(QString filename)
{
    QString string = sysfsReadString(filename);

    bool ok;
    int value = string.toInt(&ok);

    if (!ok) {
        sensordLogW() << "Failed to parse '" << string << "' to int from file " << filename;
    }

	return value;
}

// Return the number of channels
int IioAdaptor::scanElementsEnable(int device, int enable)
{
    QString elementsPath = devicePath + "scan_elements";

    QDir dir(elementsPath);
    if (!dir.exists()) {
        sensordLogW() << "Directory " << elementsPath << " doesn't exist";
        return 0;
    }

    // Find all the *_en file and write 0/1 to it
    QStringList filters;
    filters << "*_en";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        if (enable) {
            QString base = fileInfo.filePath();
            // Remove the _en
            base.chop(3);

            int index = sysfsReadInt(base + "_index");
            int bytes = deviceChannelParseBytes(base + "_type");

            devices_[device].channel_bytes[index] = bytes;
        }

        sysfsWriteInt(fileInfo.filePath(), enable);
    }

    return list.size();
}


int IioAdaptor::deviceChannelParseBytes(QString filename)
{
    QString type = sysfsReadString(filename);

    if (type.compare("le:s16/16>>0") == 0) {
        return 2;
    } else if (type.compare("le:s32/32>>0") == 0) {
        return 4;
    } else if (type.compare("le:s64/64>>0") == 0) {
        return 8;
    } else {
        sensordLogW() << "ERROR: invalid type from file " << filename << ": " << type;
    }

    return 0;
}

void IioAdaptor::processSample(int fileId, int fd)
{
    char buf[256];
    int readBytes = 0;
    int result = 0;
    int channel = fileId%IIO_MAX_DEVICE_CHANNELS;
    int device = (fileId - channel)/IIO_MAX_DEVICE_CHANNELS;

    if (device == 0) {
        readBytes = read(fd, buf, sizeof(buf));

        if (readBytes <= 0) {
            sensordLogW() << "read():" << strerror(errno);
            return;
        }

        result = atoi(buf);
        if (result == 0)
            return;
        switch(channel) {
        case 0: {
            switch (sensorType) {
            case IioAdaptor::IIO_ACCELEROMETER:
            case IioAdaptor::IIO_GYROSCOPE:
                timedData = iioXyzBuffer_->nextSlot();
                result = -(result + offset) * scale * 100;
                timedData->x_= result;
                break;
            case IioAdaptor::IIO_MAGNETOMETER:
                calData = magnetometerBuffer_->nextSlot();
                result = (result + offset) * scale;
                calData->rx_ = result;
                break;
            case IioAdaptor::IIO_ALS:
                uData = alsBuffer_->nextSlot();
                result = (result + offset) * scale;
                uData->value_ = result;
                break;
            default:
                break;
            };
        }
            break;

        case 1: {
            switch (sensorType) {
            case IioAdaptor::IIO_ACCELEROMETER:
            case IioAdaptor::IIO_GYROSCOPE:
                timedData = iioXyzBuffer_->nextSlot();
                timedData->y_= -(result + offset) * scale * 100;
                break;
            case IioAdaptor::IIO_MAGNETOMETER:
                calData = magnetometerBuffer_->nextSlot();
                result = (result * scale);
                calData->y_ = result;
                break;
            default:
                break;
            };
        }
            break;

        case 2: {
            switch (sensorType) {
            case IioAdaptor::IIO_ACCELEROMETER:
            case IioAdaptor::IIO_GYROSCOPE:
                timedData = iioXyzBuffer_->nextSlot();
                timedData->z_ = -(result + offset) * scale * 100;
                break;
            case IioAdaptor::IIO_MAGNETOMETER:
                calData = magnetometerBuffer_->nextSlot();
                result = ((result + offset) * scale) * 100;
                calData->rz_ = result;
                break;
            default:
                break;
            };
        }
            break;
        };

        if (channel == devices_[dev_accl_].channels - 1) {
            switch (sensorType) {
            case IioAdaptor::IIO_ACCELEROMETER:
            case IioAdaptor::IIO_GYROSCOPE:
                timedData->timestamp_ = Utils::getTimeStamp();
                iioXyzBuffer_->commit();
                iioXyzBuffer_->wakeUpReaders();
                break;
            case IioAdaptor::IIO_MAGNETOMETER:
                calData->timestamp_ = Utils::getTimeStamp();
                magnetometerBuffer_->commit();
                magnetometerBuffer_->wakeUpReaders();
                break;
            case IioAdaptor::IIO_ALS:
                uData->timestamp_ = Utils::getTimeStamp();
                alsBuffer_->commit();
                alsBuffer_->wakeUpReaders();
                qDebug() << "XXXXXXXXXXXXXXX<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>";
                break;
            default:
                break;
            };
        }
    }
}

bool IioAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    if (mode() == SysfsAdaptor::IntervalMode)
        return SysfsAdaptor::setInterval(value, sessionId);

    sensordLogD() << "Ignoring setInterval for " << value;

    return true;
}

//unsigned int IioAdaptor::interval() const
//{
//    int value = 100;
//    sensordLogD() << "Returning dummy value in interval(): " << value;
//    return value;
//}


bool IioAdaptor::startSensor()
{
    qDebug() << Q_FUNC_INFO;
    deviceEnable(dev_accl_, true);
    return SysfsAdaptor::startSensor();
}

void IioAdaptor::stopSensor()
{
    qDebug() << Q_FUNC_INFO;
    deviceEnable(dev_accl_, false);
    SysfsAdaptor::stopSensor();
}
