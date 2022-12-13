#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_CORE_MEMORY
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_buffer_object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_buffer_object.hpp
)

set_property(GLOBAL PROPERTY VPU_CORE_MEMORY ${VPU_CORE_MEMORY})
