#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_CORE_OS_INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_factory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver_api.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver_api.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface_imp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface_imp.cpp
)

set_property(GLOBAL PROPERTY VPU_CORE_OS_INTERFACE ${VPU_CORE_OS_INTERFACE})
