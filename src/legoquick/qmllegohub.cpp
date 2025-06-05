// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR
// GPL-3.0-only

#include "qmllegohub_p.h"
#include <QtLego/QLegoHub>
#include <QDebug>
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlLegoHubPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlLegoHub)
public:
    QString name;
};

/*!
    \qmltype LegoHub
    \nativetype QmlLegoHub
    \inqmlmodule QtLego
    \since QtLego 6.0
    \brief The LegoHub element allows connected devices to be controlled.
*/

QmlLegoHub::QmlLegoHub(QObject *parent)
    : QObject(*(new QmlLegoHubPrivate), parent)
{
}

QmlLegoHub::~QmlLegoHub() { }

/*!
    \qmlproperty string LegoHub::name
    This property holds the current hub name.

    Please see QLegoHub::name for information about this property.
*/

QString QmlLegoHub::name() const
{
    return hub()->name();
}

void QmlLegoHub::setName(const QString &name)
{
    hub()->setName(name);
}

/*!
    \qmlproperty bool LegoHub::connected
    This property holds a value indicating if the hub is connected.

    Please see QLegoHub::connected for information about this property.
*/

bool QmlLegoHub::isConnected() const
{
    return hub()->isConnected();
}

/*!
    \qmlmethod bool LegoHub::connect()
    Establish a connection to the hub. Returns true if successful, false otherwise.

    Please see QLegoHub::connect() for information.
*/

bool QmlLegoHub::connect()
{
    return hub()->connect();
}

/*!
    \qmlmethod void LegoHub::disconnect()
    Disconnect from the hub.

    Please see QLegoHub::disconnect() for information.
*/

void QmlLegoHub::disconnect()
{
    hub()->disconnect();
}

void QmlLegoHub::classBegin() { }

void QmlLegoHub::componentComplete()
{
    m_componentComplete = true;

    connect(hub(), &QLegoHub::nameChanged, this, &QmlLegoHub::nameChanged);

    Q_D(QmlLegoHub);

    if (m_activateOnComplete) {
        connect();
    }
}

QT_END_NAMESPACE
