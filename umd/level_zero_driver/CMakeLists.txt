#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME_L0 ze_intel_vpu)

# Level Zero third party detection
if(DEFINED LEVEL_ZERO_ROOT)
    get_filename_component(LEVEL_ZERO_ROOT "${LEVEL_ZERO_ROOT}" ABSOLUTE)
else()
    get_filename_component(LEVEL_ZERO_ROOT_tmp "${CMAKE_SOURCE_DIR}/third_party/level-zero" ABSOLUTE)
    # Level Zero Headers if read from the git repo are in include/core & include/tools.
    # To support the installation path of level_zero_loader headers which is include/*
    # the header files are combined into the path include/* in the commands below.
    if(IS_DIRECTORY "${LEVEL_ZERO_ROOT_tmp}")
        set(CUSTOM_L0_INCLUDE_PATH "${CMAKE_BINARY_DIR}/level_zero/")
        file(MAKE_DIRECTORY ${CUSTOM_L0_INCLUDE_PATH})
        file(GLOB LEVEL_ZERO_API_HEADERS
            "${CMAKE_SOURCE_DIR}/third_party/level-zero/include/*.h"
            "${CMAKE_SOURCE_DIR}/third_party/level-zero/include/layers"
            "${CMAKE_SOURCE_DIR}/third_party/level-zero/include/loader"
        )
        file(COPY ${LEVEL_ZERO_API_HEADERS} DESTINATION ${CUSTOM_L0_INCLUDE_PATH})
        file(GLOB LEVEL_ZERO_API_EXT_HEADERS
            "${CMAKE_SOURCE_DIR}/third_party/level-zero-vpu-extensions/*.h"
        )
        file(COPY ${LEVEL_ZERO_API_EXT_HEADERS} DESTINATION ${CUSTOM_L0_INCLUDE_PATH})
        set(LEVEL_ZERO_ROOT "${LEVEL_ZERO_ROOT_tmp}")
    endif()
endif()

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

if (CUSTOM_L0_INCLUDE_PATH)
    include_directories(${CUSTOM_L0_INCLUDE_PATH}/../)
else()
    include_directories(${LevelZero_INCLUDE_DIRS})
endif()

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
add_dependencies(${TARGET_NAME_L0} ze_loader)
