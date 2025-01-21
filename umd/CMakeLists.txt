#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(common_macros.cmake)
include(CheckCXXCompilerFlag)

if (ENABLE_INCLUDE_WHAT_YOU_USE)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
                    OUTPUT_VARIABLE CLANG_VERSION
                    COMMAND_ERROR_IS_FATAL ANY)
    if (NOT ${CLANG_VERSION} MATCHES "clang version 17")
        message(FATAL_ERROR "include-what-you-use needs clang compiler version 17.\nPlease set -DCMAKE_C_COMPILER=clang-17 -DCMAKE_CXX_COMPILER=clang++-17")
    endif()
    set(IWYU "include-what-you-use;-Xiwyu;--error")
    set(CMAKE_C_INCLUDE_WHAT_YOU_USE "${IWYU}")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU}")
endif()

if (NOT UMD_LOGLEVEL)
  set(UMD_LOGLEVEL ERROR)
endif()

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUMD_LOGLEVEL=${UMD_LOGLEVEL}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VPU_KMD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../linux/include")
set(VPU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Enable extra warning and -ftrapv that abort program on signed arithmetic overflow
add_compile_options(-Winvalid-pch -Wconversion -Wuninitialized -ftrapv)
add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)

# Project-wide include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver/include")
include_directories(${VPU_SOURCE_DIR})
include_directories(SYSTEM ${VPU_KMD_INCLUDE_DIR})

add_subdirectory_unique(vpu_driver/source)
add_subdirectory_unique(vpu_driver/unit_tests)

add_subdirectory_unique(level_zero_driver/source)
add_subdirectory_unique(level_zero_driver/unit_tests)
add_subdirectory_unique(level_zero_driver/api)
