#include "qlegomotor.h"
#include "qlegocommon.h"
#include <QString>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(motorLogger, "lego.attachedDevice.motor");

enum MotorValues
{
    Stop = 0,
    Brake = 127,
};

/*!
  \class QLegoMotor
  \brief The QLegoMotor class allows access to any motors connected to a LEGO device.
  \inmodule QtLego
  \ingroup attached-devices

  QLegoMotor is the implementation of QLegoAbstractDevice specifically for motors.
  Because motors provide no feedback, QLegoMotor is used solely for issuing commands
  to a device attached to a LEGO Hub.

  Motors can be detected by connecting to the \l{QLegoDevice::deviceAttached()} signal,
  or by calling the \l{QLegoDevice::waitForAttachedMotor()} functions.
  The former is completely asynchronous, but has no way of knowing what is being attached (or where)
  without the user querying for it. The later is synchronous but non-blocking, and
  allows selecting a specific motor.

  \sa QLegoDevice, QLegoAttachedDevice

  This example sets any attached motors to 50% power, waits 5 seconds, then stops the motor.

  \code
  QObject::connect(device, &QLegoDevice::deviceAttached, [=](QLegoAttachedDevice *attachment) {
      if (attachment->isMotor()) {
          const auto motor = static_cast<QLegoMotor*>(attachment);
          motor->setPower(50);
          device->wait(5000);
          motor->brake();
      }
  });
  \endcode

  The next example does the same thing as the example above, except this time it
  is waiting specifically for a motor to be connected to port "A".

  \code
  QLegoMotor *motor = device->waitForAttachedMotor("A");
  motor->setPower(100);
  device->wait(5000);
  motor->brake();
  \endcode
*/

/*!
    \fn void QLegoMotor::powerChanged()

    This signal is emitted when the current power of this device has been changed.
*/

/*!
    Constructs a QLegoMotor object for a given \a deviceType and \a port.

    Most users will not need to construct this class themselves.
*/
QLegoMotor::QLegoMotor(DeviceType deviceType, quint8 portId, QObject *parent)
    : QLegoAttachedDevice(deviceType, portId, parent)
    , m_power(0)
{
    setAttached(true);
    setMotor(true);
    setSensor(false);
}

/*!
    \property QLegoMotor::power
    \brief the current power level.
*/
int QLegoMotor::power() const
{
    return m_power;
}

void QLegoMotor::setPower(int power)
{
    m_power = mapSpeed(power);
    qCDebug(motorLogger) << "setPower:" << m_power;
    emit powerChanged();
    writeDirect(0x00, QByteArray(1, m_power));
}

/*!
    Commands the motor to stop.
*/
void QLegoMotor::stop()
{
    setPower(MotorValues::Stop);
}

/*!
    Commands the motor to start braking.
*/
void QLegoMotor::brake()
{
    setPower(MotorValues::Brake);
}
