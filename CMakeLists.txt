# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(QtLego
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt LEGO Libraries"
    HOMEPAGE_URL "https://github.com/Symbitic/QtLego"
    LANGUAGES CXX
)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core Bluetooth Sensors)
find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS Gui Quick QuickTest Qml Widgets)

qt_internal_project_setup()

qt_build_repo()
