#include "qlegodevice.h"
#include "qlegomotor.h"
#include "qlegocommon.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtCore/QtEndian>
#include <QtCore/QLoggingCategory>
#include <QtCore/QThread>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QLowEnergyService>
#include <QtBluetooth/QLowEnergyController>

Q_LOGGING_CATEGORY(deviceLogger, "lego.device");

using AttachedDeviceType = QLegoAttachedDevice::DeviceType;
using DeviceType = QLegoDevice::DeviceType;

static inline QLegoAttachedDevice *createAttachment(AttachedDeviceType deviceType, quint8 portId)
{
    switch (deviceType) {
        case AttachedDeviceType::MoveHubMediumLinearMotor:
            return new QLegoMotor(deviceType, portId);
        case AttachedDeviceType::Unknown:
        default:
            return new QLegoAttachedDevice(deviceType, portId);
    }
}

static inline PortMap getPortMap(DeviceType deviceType)
{
    switch (deviceType) {
        case DeviceType::BoostHub:
            return BoostPortMap;
        default:
            return PortMap();
    }
}

static inline QString getPortNameForPortId(PortMap portMap, int portId)
{
    for (const auto key : portMap.keys()) {
        if (portMap[key] == portId) {
            return key;
        }
    }
    return "";
}

/*!
  \class QLegoDevice
  \brief The QLegoDevice class is a programmable LEGO brick.
  \inmodule QtLego
  \ingroup devices

  QLegoDevice represents a central LEGO device that can receive commands.
  Currently only Powered UP Bluetooth Low Energy smart hubs are supported,
  but the interface should be expandable to EV3 and Spike Prime hubs.

  QLegoDevice is the heart of QtLego. All other classes revolve around it. It represents
  a central LEGO device that can receive commands.
  Currently only Powered UP Bluetooth Low Energy smart hubs are supported,
  but the interface should be expandable to EV3 and Spike Prime hubs.
*/

/*!
    \fn void QLegoDevice::disconnected()

    This signal is emitted when the connection to a device has been dropped or lost.
*/

/*!
    \fn void QLegoDevice::ready()

    This signal is emitted when a device is first ready for use.
*/

/*!
    \fn void QLegoDevice::button(const QLegoDevice::ButtonState state)

    This signal is emitted when button's state has changed.
*/

/*!
    \fn void batteryLevel(quint8 level);

    This signal is emitted when the battery level for this device has changed.
*/

/*!
    \fn void QLegoDevice::deviceAttached(QLegoAttachedDevice *attachment);

    This signal is emitted when a new attached device has been detected.
*/

/*!
    \fn void QLegoDevice::deviceDetached(QLegoAttachedDevice *attachment);

    This signal is emitted when an attached device has been detached.
*/

/*!
    \enum QLegoDevice::DeviceType

    The different types of supported devices.

    \value UnknownDevice  Not a recognized device.

    \value BoostHub       A LEGO Boost hub.

    \value TechnicHub     A LEGO Technic Control+ hub.
*/

/*!
    Constructs a QLegoAttachedDevice object.

    Most users will not use this function. Please use \l{QLegoDeviceScanner} instead.
*/
QLegoDevice::QLegoDevice(QObject *parent)
    : QObject(parent)
    , m_name("")
    , m_firmware("0.0.00.0000")
    , m_hardware("0.0.00.0000")
    , m_address("00:00:00:00:00:00")
    , m_battery(100)
    , m_rssi(-60)
    , m_deviceType(DeviceType::UnknownDevice)
    , m_controller(nullptr)
    , m_service(nullptr)
    , m_char()
    , m_messageBuffer()
    , m_portMap()
    , m_virtualPorts()
    , m_attachedDevices()
{
}

QLegoDevice::~QLegoDevice()
{
    if (m_controller != nullptr) {
        delete m_controller;
    }
    if (m_service != nullptr) {
        delete m_service;
    }
}

/*!
    \property QLegoDevice::name
    \brief name of this device.
*/
QString QLegoDevice::name() const
{
    return m_name;
}

/*!
    \property QLegoDevice::firmware
    \brief device firmware version.
*/
QString QLegoDevice::firmware() const
{
    return m_firmware;
}

/*!
    \property QLegoDevice::firmware
    \brief device hardware version.
*/
QString QLegoDevice::hardware() const
{
    return m_hardware;
}

/*!
    \property QLegoDevice::address
    \brief mac address.
*/
QString QLegoDevice::address() const
{
    return m_address;
}

/*!
    \property QLegoDevice::battery
    \brief battery charge level.
*/
int QLegoDevice::battery() const
{
    return m_battery;
}

/*!
    \property QLegoDevice::rssi
    \brief connection signal strength.
*/
int QLegoDevice::rssi() const
{
    return m_rssi;
}

/*!
    \property QLegoDevice::deviceType
    \brief type of device connected.
*/
QLegoDevice::DeviceType QLegoDevice::deviceType() const
{
    return m_deviceType;
}

////////////////////////////////////////////////////////////////////////////////

QLegoDevice *QLegoDevice::createDevice(const QBluetoothDeviceInfo &deviceInfo)
{
    QLegoDevice *device = new QLegoDevice();
    device->m_deviceInfo = deviceInfo;
    device->m_address = getAddress(deviceInfo);
    return device;
}

void QLegoDevice::connectToDevice()
{
    if (!m_deviceInfo.isValid()) {
        qCWarning(deviceLogger) << "Not a valid device";
        emit disconnected();
        return;
    }

    m_controller = QLowEnergyController::createCentral(m_deviceInfo);

    // clang-format off
    connect(m_controller, &QLowEnergyController::connected, this, &QLegoDevice::deviceConnected);
    connect(m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &QLegoDevice::errorReceived);
    connect(m_controller, &QLowEnergyController::disconnected, this, &QLegoDevice::deviceDisconnected);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &QLegoDevice::addLowEnergyService);
    connect(m_controller, &QLowEnergyController::discoveryFinished, this, &QLegoDevice::serviceScanDone);
    // clang-format on

    m_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
    m_controller->connectToDevice();
}

void QLegoDevice::errorReceived(QLowEnergyController::Error error)
{
    Q_UNUSED(error)
    // qWarning() << "Error: " << controller->errorString();
}

void QLegoDevice::deviceConnected()
{
    auto controller = qobject_cast<QLowEnergyController *>(sender());
    controller->discoverServices();
}

void QLegoDevice::deviceDisconnected()
{
    emit disconnected();
}

void QLegoDevice::serviceScanDone()
{
    if (!m_service) {
        emit disconnected();
        return;
    }

    const auto ids = m_deviceInfo.manufacturerIds();
    bool found = false;
    for (auto id : ids) {
        // Not 100% sure about this; others used manufacturerData[3]
        const auto data = m_deviceInfo.manufacturerData(id);
        const auto chars = data.constData();
        const quint8 ch = chars[1];
        switch (ch) {
            case ManufacturerData::MoveHub:
                m_deviceType = DeviceType::BoostHub;
                m_portMap = BoostPortMap;
                break;
            case ManufacturerData::TechnicHub:
                m_deviceType = DeviceType::TechnicHub;
                break;
            default:
                m_deviceType = DeviceType::UnknownDevice;
                break;
        }
    }
}

void QLegoDevice::addLowEnergyService(const QBluetoothUuid &uuid)
{
    auto controller = qobject_cast<QLowEnergyController *>(sender());
    auto service = controller->createServiceObject(uuid);
    if (getServiceUuid(service) != LPF2_SERVICE) {
        return;
    }
    m_service = service;
    qCDebug(deviceLogger) << "UUID:" << getServiceUuid(m_service);
    connectToService(m_service);
}

void QLegoDevice::connectToService(QLowEnergyService *service)
{
    if (service->state() == QLowEnergyService::DiscoveryRequired) {
        // clang-format off
        connect(service, &QLowEnergyService::stateChanged, this, &QLegoDevice::serviceDetailsDiscovered);
        // connect(service, &QLowEnergyService::characteristicChanged, this, &QLegoDevice::updateValue);
        // connect(service, &QLowEnergyService::descriptorWritten, this, &QLegoDevice::confirmedDescriptorWrite);
        service->discoverDetails();
        // clang-format on
    } else {
        readDeviceCharacteristics(service);
    }
}

void QLegoDevice::serviceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState != QLowEnergyService::ServiceDiscovered) {
        return;
    }

    auto service = qobject_cast<QLowEnergyService *>(sender());
    if (!service) {
        return;
    }

    readDeviceCharacteristics(service);
}

/*!
    Drops the connection to a device.
*/
void QLegoDevice::disconnect()
{
    // TODO: is disconnected() signal needed?
    QByteArray bytes;
    bytes.resize(2);
    bytes[0] = 0x02;
    bytes[1] = 0x01;
    send(bytes);
}

void QLegoDevice::requestHubPropertyValue(quint8 value)
{
    QByteArray bytes;
    bytes.resize(3);
    bytes[0] = 0x01;
    bytes[1] = value;
    bytes[2] = 0x05;
    send(bytes);
}

void QLegoDevice::requestHubPropertyReports(quint8 value)
{
    QByteArray bytes;
    bytes.resize(3);
    bytes[0] = 0x01;
    bytes[1] = value;
    bytes[2] = 0x02;
    send(bytes);
}

void QLegoDevice::send(const QByteArray &bytes)
{
    QByteArray message;
    const quint8 size = bytes.size() + 2;
    message.resize(size);
    message[0] = size;
    message[1] = 0x00;
    for (int i = 0; i < bytes.size(); i++) {
        message[i + 2] = bytes[i];
    }
    // qCDebug(deviceLogger) << "send:" << message.toHex();
    m_service->writeCharacteristic(m_char, message);
}

void QLegoDevice::readDeviceCharacteristics(QLowEnergyService *service)
{
    const QList<QLowEnergyCharacteristic> chars = service->characteristics();

    for (const QLowEnergyCharacteristic &ch : chars) {
        if (getUuid(ch.uuid()) == LPF2_CHARACTERISTIC) {
            m_char = ch;
        }
    }

    if (!m_char.isValid()) {
        emit disconnected();
        return;
    }

    auto notificationDesc = m_char.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (notificationDesc.isValid()) {
        service->writeDescriptor(notificationDesc, QByteArray::fromHex("0100"));
    }

    connect(service, &QLowEnergyService::characteristicChanged, this, &QLegoDevice::parseMessage);

    // this._bleDevice.discoverCharacteristicsForService(BLEService.LPF2_HUB);
    // this._bleDevice.subscribeToCharacteristic(BLECharacteristic.LPF2_ALL, _parseMessage);

    // Button reports
    requestHubPropertyReports(0x02);
    // Firmware
    requestHubPropertyValue(0x03);
    // Hardware
    requestHubPropertyValue(0x04);
    // RSSI
    requestHubPropertyReports(0x05);
    // Battery level
    requestHubPropertyReports(0x06);
    // MAC Address
    requestHubPropertyValue(0x0D);

    QTimer::singleShot(400, this, [this]() {
        // Wait 400 milliseconds to allow time to receive responses.
        emit ready();
    });
}

void QLegoDevice::parseMessage(const QLowEnergyCharacteristic &ch, const QByteArray &data)
{
    Q_UNUSED(ch)
    if (!data.isEmpty()) {
        m_messageBuffer += data;
    }

    if (m_messageBuffer.length() <= 0) {
        return;
    }

    const quint8 len = m_messageBuffer[0];
    if (len <= m_messageBuffer.length()) {
        const QByteArray message = m_messageBuffer.mid(0, len);
        m_messageBuffer = m_messageBuffer.mid(len);

        // qCDebug(deviceLogger) << "received message:" << message.toHex();

        const auto msg = message.constData();

        const quint8 cmd = msg[2];
        switch (cmd) {
            case 0x01:
                parseHubPropertyResponse(message);
                break;
            case 0x04:
                parsePortMessage(message);
                break;
            case 0x43:
                parsePortInformationResponse(message);
                break;
            case 0x44:
                parseModeInformationResponse(message);
                break;
            case 0x45:
                parseSensorMessage(message);
                break;
            case 0x82:
                parsePortAction(message);
                break;
            default:
                break;
        }

        if (m_messageBuffer.size() > 0) {
            parseMessage(ch, QByteArray());
        }
    }
}

void QLegoDevice::parseHubPropertyResponse(const QByteArray &message)
{
    const auto msg = message.constData();
    const auto report = message[3];
    // qCDebug(deviceLogger) << "parseHubPropertyResponse" << report << message.toHex();
    if (report == 0x02) {
        // Button press reports
        if (msg[5] == 1) {
            emit button(ButtonState::Pressed);
            return;
        } else if (msg[5] == 0) {
            emit button(ButtonState::Released);
            return;
        }
    } else if (report == 0x03) {
        // Firmware version
        m_firmware = decodeVersion(message.mid(5, 4).toHex());
        // TODO: Only version 2.0.00.0017 or later is supported.
    } else if (report == 0x04) {
        // Hardware version
        m_hardware = decodeVersion(message.mid(5, 4).toHex());
    } else if (report == 0x05) {
        // RSSI update
        int rssi = 0;
        bool ok;
        const auto signal = message.mid(5, 2).toHex().toShort(&ok, 16);
        if (ok) {
            rssi = qToBigEndian<qint8>(signal);
        }
        if (rssi) {
            m_rssi = rssi;
            // emit rssiStrength();
        }
    } else if (report == 0x0D) {
        // Primary MAC Address
        m_address = message.mid(5).toHex(':');
    } else if (report == 0x06) {
        // Battery level reports
        const quint8 battery = msg[5];
        if (battery != m_battery) {
            // qCDebug(deviceLogger) << "battery:" << battery;
            m_battery = battery;
            emit batteryLevel(battery);
        }
    }
}

void QLegoDevice::parsePortMessage(const QByteArray &message)
{
    const quint8 portId = message[3];
    const quint8 event = message[4];
    int deviceNum = 0;

    // qCDebug(deviceLogger) << "parsePortMessage:" << event << message.toHex();

    if (event) {
        const auto hexStr = message.mid(5, 2).toHex();
        bool ok;
        const quint16 hex = hexStr.toUShort(&ok, 16);
        if (ok) {
            deviceNum = qToBigEndian<quint16>(hex);
        }
    }

    const auto deviceType = static_cast<AttachedDeviceType>(deviceNum);

    switch (event) {
        case 0x00: {
            // Device detachment
            if (m_attachedDevices.contains(portId)) {
                const auto attachment = m_attachedDevices[portId];
                // TODO: Should m_attachedDevices[portId] be deleted first?
                m_attachedDevices.remove(portId);
                emit deviceDetached(attachment);
                if (m_virtualPorts.contains(portId)) {
                    const auto portName = getPortNameForPortId(m_portMap, portId);
                    if (!portName.isEmpty()) {
                        m_portMap.remove(portName);
                    }
                    m_virtualPorts.removeAll(portId);
                }
            }
            break;
        }
        case 0x01: {
            // Device attachment
            const auto attachment = createAttachment(deviceType, portId);
            if (attachment != nullptr) {
                attachDevice(portId, attachment);
            }
            break;
        }
        case 0x02: {
            // Virtual port creation
            const auto firstPortName = getPortNameForPortId(m_portMap, message[7]);
            const auto secondPortName = getPortNameForPortId(m_portMap, message[8]);
            const auto virtualPortName = firstPortName + secondPortName;
            const quint8 virtualPortId = message[3];
            m_portMap[virtualPortName] = virtualPortId;
            m_virtualPorts.append(virtualPortId);
            const auto attachment = createAttachment(deviceType, virtualPortId);
            if (attachment != nullptr) {
                attachDevice(virtualPortId, attachment);
            }
            break;
        }
        default:
            break;
    }
}

void QLegoDevice::sendPortInformationRequest(quint8 port)
{
    QByteArray batch1;
    QByteArray batch2;
    batch1.resize(3);
    batch2.resize(3);
    batch1[0] = 0x21;
    batch1[1] = port;
    batch1[2] = 0x01;
    batch2[0] = 0x21;
    batch2[1] = port;
    batch2[2] = 0x02;
    send(batch1);
    send(batch2); // Mode combinations
}

void QLegoDevice::parsePortInformationResponse(const QByteArray &message)
{
    const auto msg = message.constData();
    const quint8 port = msg[3];
    if (msg[4] == 2) {
        return;
    }
    const quint8 count = msg[6];
    qCDebug(deviceLogger) << "parsePortInformationResponse:" << message.toHex();
    /*
    for (let i = 0; i < count; i++) {
        await this._sendModeInformationRequest(port, i, 0x00); // Mode Name
        await this._sendModeInformationRequest(port, i, 0x01); // RAW Range
        await this._sendModeInformationRequest(port, i, 0x02); // PCT Range
        await this._sendModeInformationRequest(port, i, 0x03); // SI Range
        await this._sendModeInformationRequest(port, i, 0x04); // SI Symbol
        await this._sendModeInformationRequest(port, i, 0x80); // Value Format
    }
    */
}

void QLegoDevice::sendModeInformationRequest(quint8 port, quint8 mode, quint8 type)
{
    QByteArray bytes;
    bytes.resize(4);
    bytes[0] = 0x22;
    bytes[1] = port;
    bytes[2] = mode;
    bytes[3] = type;
    send(bytes);
}

void QLegoDevice::parseModeInformationResponse(const QByteArray &message)
{
    Q_UNUSED(message)
    // Doesn't set any values.
}

void QLegoDevice::parsePortAction(const QByteArray &message)
{
    const auto msg = message.constData();
    const quint8 portId = msg[3];
    qCDebug(deviceLogger) << "parsePortAction:" << portId;
    /*
    const device = this._getDeviceByPortId(portId);

    if (device) {
        const finished = (message[4] == 0x0a);
        if (finished) {
            device.finish();
        }
    }
    */
}

void QLegoDevice::parseSensorMessage(const QByteArray &message)
{
    const auto msg = message.constData();
    const quint8 portId = message[3];
    qCDebug(deviceLogger) << "parseSensorMessage:" << portId;
    /*
    const device = this._getDeviceByPortId(portId);

    if (device) {
        device.receive(message);
    }
    */
}

void QLegoDevice::attachDevice(int portId, QLegoAttachedDevice *device)
{
    if (m_attachedDevices.contains(portId) && m_attachedDevices[portId]->type() == device->type()) {
        return;
    }
    m_attachedDevices[portId] = device;
    /*
    connect(device, &QLegoAttachedDevice::command, [this](const QByteArray &data) {
        // test.
        send(data);
    });
    */
    connect(device, &QLegoAttachedDevice::command, this, &QLegoDevice::send);
    emit deviceAttached(device);
}

QLegoAttachedDevice *QLegoDevice::waitForDeviceByName(const QString &name)
{
    QLegoAttachedDevice *attachment = nullptr;
    QMetaObject::Connection connection;
    const int timeout = 5000;
    QTimer *timer = new QTimer(this);
#ifdef NO_USE_PROCESS_EVENTS
    // This may cause problems if used outside the main GUI thread?
    QEventLoop loop;
#else
    bool waiting = true;
    const QEventLoop::ProcessEventsFlags flags =
            QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents;
#endif

    // Check before we start the loop.
    if (m_portMap.contains(name)) {
        const int portId = m_portMap[name];
        return m_attachedDevices[portId];
    }

#ifdef NO_USE_PROCESS_EVENTS
    const auto callback = [this, &attachment, loop, name](QLegoAttachedDevice *device) {
        const auto portName = getPortNameForPortId(m_portMap, device->portId());
        if (!portName.isEmpty() && portName == name) {
            attachment = device;
            loop.exit();
        }
    };
    connect(timer, &QTimer::timeout, this, [loop]() { loop.exit(); });
#else
    const auto callback = [this, &attachment, &waiting, name](QLegoAttachedDevice *device) {
        const auto portName = getPortNameForPortId(m_portMap, device->portId());
        if (!portName.isEmpty() && portName == name) {
            attachment = device;
            waiting = false;
        }
    };
    connect(timer, &QTimer::timeout, this, [&waiting]() { waiting = false; });
#endif

    connection = QObject::connect(this, &QLegoDevice::deviceAttached, callback);
    timer->setSingleShot(true);
    timer->start(timeout);

#ifdef NO_USE_PROCESS_EVENTS
    loop.exec();
#else
    while (waiting) {
        QCoreApplication::processEvents(flags);
    }
#endif

    timer->stop();
    delete timer;
    QObject::disconnect(connection);

    return attachment;
}

/*!
    Waits for a motor to be connected to port \a port.

    This function is synchronous, but it doesn't block the thread.
*/
QLegoMotor *QLegoDevice::waitForAttachedMotor(const QString &port)
{
    QLegoAttachedDevice *device = waitForDeviceByName(port);
    return qobject_cast<QLegoMotor *>(device);
}

/*!
    Waits for \a usecs micro-seconds before returning.

    This function is synchronous but non-blocking. It is a convenience function intended to help
    users deal with highly asynchronous code.
*/
void QLegoDevice::wait(const int usecs)
{
#ifdef NO_USE_PROCESS_EVENTS
    QThread::usleep(usecs);
#else
    bool waiting = true;
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&waiting]() { waiting = false; });
    timer->setSingleShot(true);
    timer->start(usecs);
    while (waiting) {
        QCoreApplication::processEvents();
    }

    delete timer;
#endif
}
