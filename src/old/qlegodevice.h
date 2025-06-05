#ifndef QLEGODEVICE_H
#define QLEGODEVICE_H

#include "qlegoglobal.h"
#include "qlegoattacheddevice.h"
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyService>
#include <QtBluetooth/QLowEnergyDescriptor>

QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)
QT_FORWARD_DECLARE_CLASS(QLowEnergyCharacteristic)
QT_FORWARD_DECLARE_CLASS(QLegoMotor)

QT_BEGIN_NAMESPACE

class Q_LEGO_EXPORT QLegoDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DeviceType deviceType READ deviceType)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString firmware READ firmware)
    Q_PROPERTY(QString hardware READ hardware)
    Q_PROPERTY(QString address READ address)
    Q_PROPERTY(int battery READ battery)
    Q_PROPERTY(int rssi READ rssi)

public:
    explicit QLegoDevice(QObject *parent = nullptr);
    ~QLegoDevice();

    static QLegoDevice *createDevice(const QBluetoothDeviceInfo &deviceInfo);

    enum DeviceType
    {
        UnknownDevice = 0,
        BoostHub = 2,
        TechnicHub = 6
    };
    Q_ENUM(DeviceType)

    enum ButtonState
    {
        Released = 0,
        Pressed = 2,
        Up = 1,
        Down = 255,
        Stop = 127
    };
    Q_ENUM(ButtonState)

    QString name() const;
    QString firmware() const;
    QString hardware() const;
    QString address() const;
    int battery() const;
    int rssi() const;
    DeviceType deviceType() const;

    Q_INVOKABLE QLegoMotor *waitForAttachedMotor(const QString &port);
    // Q_INVOKABLE QLegoMotor *waitForAttachedMotor(const DeviceType deviceType);
    // Q_INVOKABLE QLegoSensor *waitForAttachedSensor(const QString &name);
    // Q_INVOKABLE QLegoSensor *waitForAttachedSensor(const DeviceType deviceType);

    QLegoAttachedDevice *waitForDeviceByName(const QString &name);
    // Q_INVOKABLE QLegoAttachedDevice* waitForDeviceByType(const DeviceType deviceType);

public Q_SLOTS:
    void connectToDevice();
    void disconnect();

    void wait(const int usecs);

private Q_SLOTS:
    void addLowEnergyService(const QBluetoothUuid &uuid);
    void deviceConnected();
    void errorReceived(QLowEnergyController::Error error);
    void serviceScanDone();
    void deviceDisconnected();
    void serviceDetailsDiscovered(QLowEnergyService::ServiceState newState);
    void parseMessage(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void send(const QByteArray &bytes);

Q_SIGNALS:
    void disconnected();
    void ready();
    void button(const ButtonState state);
    void batteryLevel(quint8 level);
    void deviceAttached(QLegoAttachedDevice *attachment);
    void deviceDetached(QLegoAttachedDevice *attachment);

#if 0
private Q_SLOTS:
    void waitTimeout();
#endif

private:
    void connectToService(QLowEnergyService *service);
    void readDeviceCharacteristics(QLowEnergyService *service);
    void requestHubPropertyValue(quint8 value);
    void requestHubPropertyReports(quint8 value);
    void parseHubPropertyResponse(const QByteArray &message);
    void parsePortMessage(const QByteArray &message);
    void parsePortInformationResponse(const QByteArray &message);
    void parseModeInformationResponse(const QByteArray &message);
    void parseSensorMessage(const QByteArray &message);
    void parsePortAction(const QByteArray &message);
    void sendPortInformationRequest(quint8 port);
    void sendModeInformationRequest(quint8 port, quint8 mode, quint8 type);
    void attachDevice(int portId, QLegoAttachedDevice *device);

    QString m_name;
    QString m_firmware;
    QString m_hardware;
    QString m_address;
    quint8 m_battery;
    int m_rssi;
    DeviceType m_deviceType;
    QBluetoothDeviceInfo m_deviceInfo;
    QLowEnergyController *m_controller;
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_char;
    QByteArray m_messageBuffer;
    QMap<QString, int> m_portMap;
    QList<int> m_virtualPorts;
    QMap<int, QLegoAttachedDevice *> m_attachedDevices;
};

QT_END_NAMESPACE

#endif
