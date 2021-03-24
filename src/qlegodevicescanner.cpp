#include "qlegodevicescanner.h"
#include "qlegocommon.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QMetaEnum>
#include <QtCore/QString>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>

Q_LOGGING_CATEGORY(scannerLogger, "lego.scanner");

// TODO: Need to verify full names before can use.
static QStringList deviceNames({ "LEGO Move Hub", "Technic" });

/*!
  \class QLegoDeviceScanner
  \brief The QLegoDeviceScanner class scans for QLegoDevices.
  \inmodule QtLego
  \ingroup devices

  Users will use QLegoDeviceScanner to scan for and detect QLegoDevice instances, rather than
  create new device instances themselves.

  QLegoDeviceScanner currently only scans using Bluetooth LE, but future versions
  may support scanning by Bluetooth Classic or serial ports.

  \section1 Scanning and Connecting

  When a new device has been detected, the \l{QLegoDeviceScanner::deviceFound()} signal will
  be emitted to notify that a new device has been connected. This is the primary way for
  users to access QLegoDevice instances, instead of creating it themselves.

  \section1 Example

  The following example scans for new devices, then prints the address and firmware version
  of each device detected before waiting 10 seconds before disconnecting.

  \code
  auto scanner = new QLegoDeviceScanner();

  QObject::connect(scanner, &QLegoDeviceScanner::deviceFound, [=](QLegoDevice *device) {
      qDebug() << "  Address:" << device->address();
      qDebug() << "  Firmware:" << device->firmware();
      QTimer::singleShot(10000, device, &QLegoDevice::disconnect);
  });
  \endcode
*/

/*!
    \fn void QLegoDevice::errorMessage(const QString &msg)

    This signal is emitted when an error has occurred.
*/

/*!
    \fn void QLegoDevice::deviceFound(QLegoDevice *device);

    This signal is emitted when a new device has been detected.
*/

/*!
    \fn void QLegoDevice::finished();

    This signal is emitted when the scanner has finished searching for devices.
*/

/*!
    Constructs a QLegoDeviceScanner object.
*/
QLegoDeviceScanner::QLegoDeviceScanner(QObject *parent)
    : QObject(parent)
    , m_agent(new QBluetoothDeviceDiscoveryAgent)
    , m_scanning(false)
    , m_deviceCount(0)
{
    m_agent->setLowEnergyDiscoveryTimeout(5000);

    // clang-format off
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &QLegoDeviceScanner::addDevice);
    connect(m_agent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &QLegoDeviceScanner::deviceScanError);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &QLegoDeviceScanner::deviceScanFinished);
    // clang-format on
}

QLegoDeviceScanner::~QLegoDeviceScanner()
{
    delete m_agent;
}

/*!
    \property QLegoDeviceScanner::scanning
    \brief returns whether a scan is currently in progress.
*/
bool QLegoDeviceScanner::scanning() const
{
    return m_scanning;
}

/*!
    Begin scanning for devices.
*/
void QLegoDeviceScanner::scan()
{
    m_scanning = true;
    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void QLegoDeviceScanner::addDevice(const QBluetoothDeviceInfo &info)
{
    /*
     * **TODO**: NEED TO FIX THE NAMES!!!
     */
    if (info.name().contains("Move Hub", Qt::CaseInsensitive) || info.name().contains("Technic")) {
        qCDebug(scannerLogger) << "found" << info.name();

#if 0
        const auto address = getAddress(info);
        if (m_addresses.contains(address)) {
            qDebug() << "Device" << address << "already added";
            // return;
        }
#endif

        QLegoDevice *device = QLegoDevice::createDevice(info);

        QObject::connect(device, &QLegoDevice::disconnected, [this, device]() {
            m_deviceCount = m_deviceCount > 0 ? m_deviceCount - 1 : 0;
#if 0
            const auto addr = device->address();
            m_addresses.removeAll(addr);
            if (m_addresses.isEmpty()) {
                qDebug() << "No more devices";
                emit done();
            }
#endif
            device->deleteLater();
        });

#if 0
        m_addresses.append(address);

        device->connectToDevice();
#endif

        QObject::connect(device, &QLegoDevice::ready, [this, device]() {
            // TODO: Maybe disconnect() ?
            emit deviceFound(device);
        });

        m_deviceCount++;

        device->connectToDevice();

#if 0
        // Deactivate. KEEP! Will use when finished with service scan.
        const auto ids = info.manufacturerIds();
        bool found = false;
        for (auto id : ids) {
            // Not 100% sure about this; others used manufacturerData[3]
            const auto data = info.manufacturerData(id);
            const auto chars = data.constData();
            const unsigned char ch = chars[1];
            switch (ch) {
                case ManufacturerData::MoveHub:

                case ManufacturerData::TechnicHub:

            }
        }
#endif
    }
}

void QLegoDeviceScanner::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError) {
        emit errorMessage("The Bluetooth adaptor is powered off.");
    } else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError) {
        emit errorMessage("Writing or reading from the device resulted in an error.");
    } else {
        const auto index = m_agent->metaObject()->indexOfEnumerator("Error");
        static QMetaEnum qme = m_agent->metaObject()->enumerator(index);
        emit errorMessage("Error: " + QLatin1String(qme.valueToKey(error)));
    }
}

void QLegoDeviceScanner::deviceScanFinished()
{
    m_scanning = false;
    emit finished();
#if 0
    if (m_addresses.size() == 0) {
        qDebug() << "No devices found";
        emit done();
    }
#endif
}

/*!
    \property QLegoDeviceScanner::devicesFound
    \brief the number of devices the scanner has detected.
*/
int QLegoDeviceScanner::devicesFound() const
{
    return m_deviceCount;
}

/*
https://github.com/nathankellenicki/node-poweredup/blob/master/src/consts.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/nobleabstraction.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/poweredup-node.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/utils.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/hubs/basehub.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/hubs/lpf2hub.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/hubs/movehub.ts
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/src/bluetooth
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/src/bluetooth/bluez
https://doc.qt.io/qt-5/qbluetoothdeviceinfo.html#manufacturerData
https://doc.qt.io/qt-5/qtbluetooth-module.html
https://doc.qt.io/qt-5/qlowenergycontroller.html
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner?h=5.15
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner/deviceinfo.cpp?h=5.15
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner/device.h?h=5.15
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner/device.cpp?h=5.15
https://doc.qt.io/qt-5/qlowenergycontroller.html
https://doc.qt.io/qt-5/qbluetoothdeviceinfo.html
https://doc.qt.io/qt-5/qbluetoothaddress.html
https://doc.qt.io/qt-5/qtbluetooth-le-overview.html
https://lego.github.io/lego-ble-wireless-protocol-docs/

public subscribeToCharacteristic (uuid: string, callback: (data: Buffer) => void) {
        uuid = this._sanitizeUUID(uuid);
        this._characteristics[uuid].on("data", (data: Buffer) => {
            return callback(data);
        });
        this._characteristics[uuid].subscribe((err) => {
            if (err) {
                throw new Error(err);
            }
        });
}
*/
