#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_CORE_UTILITIES
    ${CMAKE_CURRENT_SOURCE_DIR}/log.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp
)

set_property(GLOBAL PROPERTY VPU_CORE_UTILITIES ${VPU_CORE_UTILITIES})
