#ifndef QLEGOATTACHEDDEVICE_H
#define QLEGOATTACHEDDEVICE_H

#include "qlegoglobal.h"

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QString)

QT_BEGIN_NAMESPACE

class Q_LEGO_EXPORT QLegoAttachedDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DeviceType type READ type)
    Q_PROPERTY(bool attached READ attached)
    Q_PROPERTY(bool sensor READ sensor)
    Q_PROPERTY(bool motor READ motor)
    Q_PROPERTY(int portId READ portId)

public:
    enum DeviceType
    {
        Unknown = 0,
        MoveHubMediumLinearMotor = 39,
        SimpleMediumLinearMotor = 1,
        TrainMotor = 2,
        Light = 8,
        VoltageSensor = 20,
        CurrentSensor = 21,
        PiezoBuzzer = 22,
        HubLed = 23,
        TiltSensor = 34,
        MotionSensor = 35,
        ColorDistanceSensor = 37,
        MediumLinearMotor = 38,
        MoveHubTiltSensor = 40,
        DuploTrainMotor = 41,
        DuploTrainSpeaker = 42,
        DuploTrainColorSensor = 43,
        DuploTrainSpeedometer = 44,
        TechnicLargeLinearMotor = 46, // Technic Control+
        TechnicXLargeLinearMotor = 47, // Technic Control+
        SpikePrimeMediumAngularMotor = 48,
        SpikePrimeLargeAngularMotor = 49,
        TechnicMediumHubGestSensor = 54,
        RemoteControlButton = 55,
        RemoteControlRssi = 56,
        TechnicMediumHubAccelerometer = 57,
        TechnicMediumHubGyroSensor = 58,
        TechnicMediumHubTiltSensor = 59,
        TechnicMediumHubTemperatureSensor = 60,
        SpikePrimeColorSensor = 61,
        SpikePrimeDistanceSensor = 62,
        SpikePrimeForceSensor = 63,
        TechnicMediumAngularMotor = 75, // Technic Control+
        TechnicLargeAngularMotor = 76, // Technic Control+
    };
    Q_ENUM(DeviceType)

    QLegoAttachedDevice(DeviceType deviceType, quint8 portId, QObject *parent = nullptr);

    DeviceType type() const;
    bool attached() const;
    bool sensor() const;
    bool motor() const;
    int portId() const;

public Q_SLOTS:
    void detach();

Q_SIGNALS:
    // Signals the parent object to send a command to the device.
    void command(const QByteArray &command);

protected:
    void setDeviceType(DeviceType type);
    void setAttached(bool attached);
    void setSensor(bool sensor);
    void setMotor(bool motor);

    void writeDirect(quint8 mode, const QByteArray &data);

private:
    DeviceType m_type;
    bool m_attached;
    bool m_sensor;
    bool m_motor;
    quint8 m_portId;
};

QT_END_NAMESPACE

#endif
