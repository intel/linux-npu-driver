#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME_L0_DRV level_zero_driver)

add_library(${TARGET_NAME_L0_DRV} STATIC)
set_property(TARGET ${TARGET_NAME_L0_DRV} PROPERTY POSITION_INDEPENDENT_CODE ON)

find_package(OpenSSL REQUIRED)
target_link_libraries(${TARGET_NAME_L0_DRV}
    fw_vpu_api_headers
    vpu_driver
    vpux_elf
    OpenSSL::SSL
)

string(TIMESTAMP UNIX_SECONDS %s UTC)
set(DRIVER_VERSION ${UNIX_SECONDS} CACHE STRING "Decimal value that set Level Zero driverVersion")
message(STATUS "Level Zero driver version: ${DRIVER_VERSION}")

target_compile_definitions(${TARGET_NAME_L0_DRV} PUBLIC DRIVER_VERSION=${DRIVER_VERSION})

add_subdirectory(core/source)
add_subdirectory(ext/source)
add_subdirectory(tools/source)
