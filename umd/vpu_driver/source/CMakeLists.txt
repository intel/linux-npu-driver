#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME vpu_driver)

add_library(${TARGET_NAME} STATIC)
set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${TARGET_NAME} fw_vpu_api_headers)

add_subdirectories()

if(ENABLE_VPUX_COMPILER)
  add_dependencies(${TARGET_NAME} vpux_compiler)
  target_link_libraries(${TARGET_NAME}
                        ${CMAKE_BINARY_DIR}/lib/libVPUXCompilerL0.so)
endif()
