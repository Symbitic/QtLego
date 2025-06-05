# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(lego)
if(TARGET Qt::Quick)
    add_subdirectory(legoquick)
endif()
