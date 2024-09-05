#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME vpu_driver)

add_library(${TARGET_NAME} STATIC)
set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(${TARGET_NAME} PRIVATE $<TARGET_PROPERTY:vpux_elf,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(${TARGET_NAME} fw_vpu_api_headers npu_compiler)

add_subdirectories()
