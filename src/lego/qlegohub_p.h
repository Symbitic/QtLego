// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QLEGOHUB_P_H
#define QLEGOHUB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qlegohub.h"

#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

class QLegoHubPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QLegoHub)
public:
    QLegoHubPrivate()
        : name()
        , connected(false)
    {
    }

    //void init(const QByteArray &sensorType);

    QString name;
    bool connected;
    int error;
};

QT_END_NAMESPACE

#endif