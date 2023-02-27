#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/log.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp
)
