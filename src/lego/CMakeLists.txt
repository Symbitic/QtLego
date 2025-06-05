# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## LEGO Module:
#####################################################################

qt_internal_add_module(Lego
    SOURCES
        qlegoglobal.h
        qlegohub.cpp qlegohub.h
    LIBRARIES
        Qt::CorePrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Bluetooth
        Qt::Sensors
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
)

## Scopes:
#####################################################################

qt_internal_add_docs(Lego
    doc/qtlego.qdocconf
)

if(QT_FEATURE_doc_snippets)
    add_subdirectory(doc/snippets/lego)
endif()
