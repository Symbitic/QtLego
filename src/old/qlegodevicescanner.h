#ifndef QLEGODEVICESCANNER_H
#define QLEGODEVICESCANNER_H

#include "qlegoglobal.h"
#include "qlegodevice.h"

#include <QtCore/QObject>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>

QT_BEGIN_NAMESPACE

QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)

class Q_LEGO_EXPORT QLegoDeviceScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning)
    Q_PROPERTY(int devicesFound READ devicesFound)
public:
    explicit QLegoDeviceScanner(QObject *parent = nullptr);
    ~QLegoDeviceScanner();

    bool scanning() const;
    int devicesFound() const;

    Q_INVOKABLE void scan();

private Q_SLOTS:
    void addDevice(const QBluetoothDeviceInfo &info);
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void deviceScanFinished();

Q_SIGNALS:
    void errorMessage(const QString &msg);
    void deviceFound(QLegoDevice *device);
    void finished();

private:
    bool m_scanning;
    int m_deviceCount;
    QBluetoothDeviceDiscoveryAgent *m_agent;
};

QT_END_NAMESPACE

#endif
