#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_context.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hw_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric_info.hpp
)

add_subdirectories()
