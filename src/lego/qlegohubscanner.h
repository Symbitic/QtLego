// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QLEGOHUBSCANNER_H
#define QLEGOHUBSCANNER_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QtLego/qlegoglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QLegoHubScannerPrivate;

class Q_LEGO_EXPORT QLegoHubScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)

public:
    enum QueryFlag {
        QueryLpf2 = 0x01,
        QueryAll = 0xFF
    };
    Q_DECLARE_FLAGS(QueryFlags, QueryFlag)

    QLegoHubScanner(QObject *parent = nullptr);
    ~QLegoHubScanner();

    bool scanning() const;

public slots:
    bool startScan(QueryFlags flags = QueryAll);
    void stop();

private slots:
    void addDevice(const QBluetoothDeviceInfo &device);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();

signals:
    void scanningChanged(bool value);

private:
    Q_DISABLE_COPY(QLegoHubScanner)
    Q_DECLARE_PRIVATE(QLegoHubScanner)
    QLegoHubScannerPrivate *d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QLegoHubScanner::QueryFlags)

QT_END_NAMESPACE

#endif
