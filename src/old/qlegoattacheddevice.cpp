#include "qlegoattacheddevice.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QString>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QLowEnergyService>

Q_LOGGING_CATEGORY(attachedDeviceLogger, "lego.attachedDevice");

/*!
  \class QLegoAttachedDevice
  \brief The QLegoAttachedDevice class is a device attached to a QLegoDevice (aka Hub).
  \inmodule QtLego
  \ingroup attached-devices

  QLegoAttachedDevice is an abstract device meant to be implemented by specific types of devices
  (currently motors and sensors).

  An attached device represents a connection of a specific type of device to a specific port on
  the parent device.

  \note Users should NEVER create a QLegoAttachedDevice directly. This API will
  likely change significantly.
*/

/*!
    \enum QLegoAttachedDevice::DeviceType

    Which type of device is currently connected.

    \value Unknown                   Not a recognized device.

    \value MoveHubMediumLinearMotor  The built-in motor for Boost Move hubs.
*/

/*!
    Constructs a QLegoAttachedDevice object.

    \warning Users should not create a new attached device instance themselves.
*/
QLegoAttachedDevice::QLegoAttachedDevice(DeviceType type, quint8 portId, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_attached(false)
    , m_sensor(false)
    , m_motor(false)
    , m_portId(portId)
{
}

/*!
    \property QLegoAttachedDevice::type
    \brief which device is currently connected.
*/
QLegoAttachedDevice::DeviceType QLegoAttachedDevice::type() const
{
    return m_type;
}

/*!
    \property QLegoAttachedDevice::attached
    \brief returns whether the device is still attached.
*/
bool QLegoAttachedDevice::attached() const
{
    return m_attached;
}

/*!
    \property QLegoAttachedDevice::sensor
    \brief returns whether the device is an instance of a sensor.
*/
bool QLegoAttachedDevice::sensor() const
{
    return m_sensor;
}

/*!
    \property QLegoAttachedDevice::motor
    \brief returns whether the device is an instance of a motor.
*/
bool QLegoAttachedDevice::motor() const
{
    return m_motor;
}

int QLegoAttachedDevice::portId() const
{
    return m_portId;
}

/*!
    Detaches an attached device. The connection to the port is dropped, but
    physically the device remains attached until the user removes it.
*/
void QLegoAttachedDevice::detach()
{
    setAttached(false);
}

void QLegoAttachedDevice::setDeviceType(QLegoAttachedDevice::DeviceType type)
{
    m_type = type;
}

void QLegoAttachedDevice::setAttached(bool attached)
{
    m_attached = attached;
}

void QLegoAttachedDevice::setSensor(bool sensor)
{
    m_sensor = sensor;
}

void QLegoAttachedDevice::setMotor(bool motor)
{
    m_motor = motor;
}

void QLegoAttachedDevice::writeDirect(quint8 mode, const QByteArray &data)
{
    QByteArray bytes;
    const quint8 size = data.size() + 5;
    bytes.resize(size);
    bytes[0] = 0x81;
    bytes[1] = m_portId;
    bytes[2] = 0x11;
    bytes[3] = 0x51;
    bytes[4] = mode;
    for (int i = 0; i < data.size(); i++) {
        bytes[i + 5] = data[i];
    }

    qCDebug(attachedDeviceLogger) << "writeDirect:" << bytes.toHex();
    emit command(bytes);
}
