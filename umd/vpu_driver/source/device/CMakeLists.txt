#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_CORE_DEVICE
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_context.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/devicelist.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_info_mtl.hpp
)

set_property(GLOBAL PROPERTY VPU_CORE_DEVICE ${VPU_CORE_DEVICE})
