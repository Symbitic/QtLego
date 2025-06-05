// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR
// GPL-3.0-only

#ifndef QLEGOHUB_H
#define QLEGOHUB_H

#include <QtLego/qlegoglobal.h>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

class QLegoHubPrivate;
class QLegoHubBackend;

class Q_LEGO_EXPORT QLegoHub : public QObject
{
    friend class QLegoHubBackend;
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool connected READ isConnected)
public:
    explicit QLegoHub(QObject *parent = nullptr);
    ~QLegoHub();

    QString name() const;
    void setName(const QString &name);

    bool isConnected() const;

public Q_SLOTS:
    bool connect();
    void disconnect();

Q_SIGNALS:
    void nameChanged();
    void connectedChanged();

protected:
    explicit QLegoHub(QLegoHubPrivate &dd, QObject *parent = nullptr);
    QLegoHubBackend *backend() const;

private:
    void registerInstance();

    Q_DISABLE_COPY(QLegoHub)
    Q_DECLARE_PRIVATE(QLegoHub)
};

QT_END_NAMESPACE

#endif
