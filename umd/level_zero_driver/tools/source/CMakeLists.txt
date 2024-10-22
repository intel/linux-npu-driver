#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME_L0_DRV} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_query.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_query.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_streamer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_streamer.cpp
)
