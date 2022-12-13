#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_CORE_tests_mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_vpu_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_vpu_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gmock_os_interface_imp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_os_interface_imp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_os_interface_imp.cpp
)

set_property(GLOBAL PROPERTY VPU_CORE_tests_mocks ${VPU_CORE_tests_mocks})
