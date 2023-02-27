#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(L0_TOOLS_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_query.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_query.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_streamer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics/metric_streamer.cpp
)

# Make our source files visible to parent
set_property(GLOBAL PROPERTY L0_TOOLS_SOURCE ${L0_TOOLS_SOURCE})
