#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/test_cmdlist.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/test_cmdlist_api.cpp
)
