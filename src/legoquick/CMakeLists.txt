# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_qml_module(LegoQuick
    URI "QtLego"
    VERSION "${PROJECT_VERSION}"
    SOURCES
        qmllegohub.cpp qmllegohub_p.h
        qlegoquickglobal_p.h
    DEFINES
        QT_BUILD_LEGOQUICK_LIB
    DEPENDENCIES
        QtQml
    PUBLIC_LIBRARIES
        Qt::Bluetooth
        Qt::Core
        Qt::Lego
        Qt::Qml
        Qt::Sensors
    LIBRARIES
        Qt::CorePrivate
)
