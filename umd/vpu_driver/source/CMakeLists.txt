#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME vpu_driver)

add_library(${TARGET_NAME} STATIC)

if(NOT ENABLE_OFFLINE_COMPILATION_SUPPORT)
	target_compile_definitions(${TARGET_NAME} PRIVATE NO_NULL_INTERFACE)
endif()

set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(${TARGET_NAME} fw_vpu_api_headers npu_elf npu_compiler)

add_subdirectories()
