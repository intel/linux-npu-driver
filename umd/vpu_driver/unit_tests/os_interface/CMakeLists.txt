#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_CORE_OS_INTERFACE_TESTS_LINUX
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver_api_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_factory_test.cpp
)

set_property(GLOBAL PROPERTY VPU_CORE_OS_INTERFACE_TESTS_LINUX ${VPU_CORE_OS_INTERFACE_TESTS_LINUX})

target_sources(${TARGET_NAME} PRIVATE
    ${VPU_CORE_OS_INTERFACE_TESTS_LINUX}
)
