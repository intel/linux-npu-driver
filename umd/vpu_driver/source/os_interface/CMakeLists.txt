#
# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT
#
set(VPU_CORE_OS_INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_factory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver_api.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver_api.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface_imp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os_interface_imp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver_ioctl_trace.cpp
)

if(ENABLE_OFFLINE_COMPILATION_SUPPORT)
    set(VPU_CORE_OS_INTERFACE
        ${VPU_CORE_OS_INTERFACE}
        ${CMAKE_CURRENT_SOURCE_DIR}/null_interface_imp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/null_interface_imp.cpp)
endif()

set_property(GLOBAL PROPERTY VPU_CORE_OS_INTERFACE ${VPU_CORE_OS_INTERFACE})

target_sources(${TARGET_NAME} PRIVATE ${VPU_CORE_OS_INTERFACE})
