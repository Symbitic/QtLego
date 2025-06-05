// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR
// GPL-3.0-only

#ifndef QMLLEGOHUB_P_H
#define QMLLEGOHUB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qlegoquickglobal_p.h"

#include <QObject>
#include <QProperty>
#include <QQmlParserStatus>
#include <QtQml/qqml.h>
#include <QQmlListProperty>
#include <QtLego/QLegoHub>

QT_BEGIN_NAMESPACE

class QLegoHub;
class QmlLegoHubPrivate;

class Q_LEGOQUICK_EXPORT QmlLegoHub : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlLegoHub)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

    QML_NAMED_ELEMENT(LegoHub)
public:
    explicit QmlLegoHub(QObject *parent = 0);
    ~QmlLegoHub();

    QString name() const;
    void setName(const QString &name);

    bool isConnected() const;

    virtual QLegoHub *hub() const = 0;

    void componentComplete() override;

public Q_SLOTS:
    bool connect();
    void disconnect();

Q_SIGNALS:
    void nameChanged();
    void connectedChanged();

private:
    void classBegin() override;
    bool m_componentComplete = false;
    bool m_activateOnComplete = false;
};

QT_END_NAMESPACE

#endif
