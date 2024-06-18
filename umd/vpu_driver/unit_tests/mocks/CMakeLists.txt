#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

add_library(vpu_driver_mocks OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_vpu_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_vpu_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gmock_os_interface_imp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_os_interface_imp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_os_interface_imp.cpp
)
target_link_libraries(vpu_driver_mocks fw_vpu_api_headers gmock)
