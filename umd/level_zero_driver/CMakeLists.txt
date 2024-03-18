#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME_L0 ze_intel_vpu)

string(TIMESTAMP UNIX_SECONDS %s UTC)
set(DRIVER_VERSION ${UNIX_SECONDS} CACHE STRING "Decimal value that set Level Zero driverVersion")
add_compile_definitions(DRIVER_VERSION=${DRIVER_VERSION})
message(STATUS "Level Zero driver version: ${DRIVER_VERSION}")

add_library(${TARGET_NAME_L0} SHARED)

target_link_libraries(${TARGET_NAME_L0}
    vpu_driver
    dl
    vpux_elf
)

set_target_properties(${TARGET_NAME_L0} PROPERTIES
    FOLDER ${TARGET_NAME_L0}
    VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
)

add_subdirectories()
add_subdirectory(core/source)
add_subdirectory(ext/source)
add_subdirectory(tools/source)

target_include_directories(${TARGET_NAME_L0} PUBLIC ddi)

install(TARGETS ${TARGET_NAME_L0}
        COMPONENT level-zero-npu)
