// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR
// GPL-3.0-only

#include "qlegohub.h"
#include "qlegohub_p.h"
#include <QDebug>
#include <QMetaProperty>

QT_BEGIN_NAMESPACE

#if 0
static void registerTypes()
{
    qRegisterMetaType<qrange>("qrange");
    qRegisterMetaType<qrangelist>("qrangelist");
    qRegisterMetaType<qoutputrangelist>("qoutputrangelist");
}
Q_CONSTRUCTOR_FUNCTION(registerTypes)

void QSensorPrivate::init(const QByteArray &sensorType)
{
    Q_Q(QSensor);
    type = sensorType;
    q->registerInstance(); // so the availableSensorsChanged() signal works
}
#endif

/*!
    \class QLegoHub
    \ingroup lego_hub
    \inmodule QtLego
    \since 6.8

    \brief The QLegoHub class represents a single LEGO smart hub.

    The lifecycle of a sensor is typically:

    \list
    \li Create a QLegoHub instance on the stack or heap.
    \li Connect to a LEGO hub.
    \li The application uses Sensor data and sends Device commands.
    \li Disconnect.
    \endlist

    QtLego uses QtSensors to provide access to sensors by dynamically registering a custom sensor
    type every time a hub is connected.

    \section1 Sensors

    Sensors are classified as LEGO devices attached to a LEGO hub.

    \sa QSensor
*/

/*!
    \enum QLegoHub::Feature
    \brief Lists optional features a LEGO Hub might support.

    The features of the LEGO Technic Hub are:

    \value Accelerometer The hub has a built-in accelerometer.

    \omitvalue Reserved

    \since 6.8
*/

/*!
    Construct a new hub.
*/
QLegoHub::QLegoHub(QObject *parent)
    : QObject(*new QLegoHubPrivate, parent)
{
    Q_D(QLegoHub);
    // d->init(type);
}

/*! \internal
 */
QSensor::QSensor(QSensorPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QLegoHub);
    // d->init(type);
}

/*! \internal
 */
QLegoHubBackend *QLegoHub::backend() const
{
    Q_D(const QLegoHub);
    return d->backend;
}

/*!
    Destroy the hub instance. Disconnects the hub if not already done so.
*/
QLegoHub::~QLegoHub()
{
    Q_D(QLegoHub);
    disconnect();
    delete d->backend;
    d->backend = 0;
}

/*!
    \property QLegoHub::connected
    \brief a value indicating if the hub is connected.

    No information about a hub is reliable until a connection has been
    established.
*/

bool QLegoHub::isConnected() const
{
    Q_D(const QLegoHub);
    return (d->backend != 0);
}

/*!
    \property QLegoHub::name
    \brief the hub name.

    Note that name can be changed by a user.
*/

QString QLegoHub::name() const
{
    Q_D(const QLegoHub);
    return d->name;
}

void QLegoHub::setName(const QString &name)
{
    Q_D(QLegoHub);
    if (d->name == name || !isConnected())
        return;
    d->backend->setName(name);
    emit nameChanged();
}

/*!
    Establish a connection.

    Returns true if the hub was successfully connected, false otherwise.

    \sa isConnected()
*/
bool QLegoHub::connect()
{
    Q_D(QSensor);
    if (isConnected()) {
        return true;
    }

    // TODO
    // d->backend = QSensorManager::createBackend(this);

    if (isConnected()) {
        d->connected = true;
        Q_EMIT connectedChanged();
        return true;
    }

    return false;
}

/*!
    Disconnect from the hub.
*/
void QLegoHub::disconnect()
{
    Q_D(QSensor);
    if (!isConnected())
        return;
    d->connected = false;
    d->backend->disconnect();
    Q_EMIT connectedChanged();
}

#if 0
/*!
   Checks if a specific feature is supported by the backend.

   QtSensors supports a rich API for controlling and providing information about sensors. Naturally,
   not all of this functionality can be supported by all of the backends.

   To check if the current backend supports the feature \a feature, call this function.

   The backend needs to be connected, otherwise false will be returned. Calling connectToBackend()
   or start() will create a connection to the backend.

   Backends have to implement QSensorBackend::isFeatureSupported() to make this work.

   Returns whether or not the feature is supported if the backend is connected, or false if the backend is not connected.
   \since 5.0
 */
bool QSensor::isFeatureSupported(Feature feature) const
{
    Q_D(const QSensor);
    return d->backend && d->backend->isFeatureSupported(feature);
}
#endif

/*!
    \fn QLegoHub::connectedChanged()

    This signal is emitted when the QLegoHub::connected property has changed.

    \sa QLegoHub::connected
*/

/*!
    \fn QLegoHub::availableHubsChanged()

    This signal is emitted when the list of available hubs has changed.

    \sa QLegoHub::hubs()
*/

QT_END_NAMESPACE