#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME_L0 ze_intel_vpu)

project(level-zero-vpu VERSION 1.1.0)
message(STATUS "Level Zero driver version: ${PROJECT_VERSION}")

# VPU UMD Version
# Note: Max value: 2 bytes / 65535
if(NOT DEFINED VPU_VERSION_BUILD)
    set(VPU_VERSION_BUILD 0)
    string(TIMESTAMP VPU_VERSION_BUILD "%y%U%w")
endif()

add_compile_definitions(
    L0_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    L0_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    VPU_VERSION_BUILD=${VPU_VERSION_BUILD})

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
        COMPONENT ${PROJECT_NAME})
