#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(common_macros.cmake)
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)

if (NOT UMD_LOGLEVEL)
  set(UMD_LOGLEVEL ERROR)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUMD_LOGLEVEL=${UMD_LOGLEVEL}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VPU_KMD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../linux/include")
set(VPU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Project-wide include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver/include")
include_directories(${VPU_SOURCE_DIR})
include_directories(SYSTEM ${VPU_KMD_INCLUDE_DIR})

# Cmake Setting - compiler independant
add_compile_options(-Wall -Wextra -Werror -Wempty-body -Wignored-qualifiers -Wtype-limits
                    -Wno-unused-parameter -Winvalid-pch -Wconversion -ftrapv -Wuninitialized)

CHECK_CXX_COMPILER_FLAG("-Wclobbered" COMPILER_SUPPORTS_WCLOBBERED)
if(COMPILER_SUPPORTS_WCLOBBERED)
    add_compile_options(-Wclobbered)
endif()

if(${CMAKE_CXX_COMPILER} MATCHES "clang")
    add_compile_options(-Wno-sign-conversion)
endif()

if(NOT ${BUILD_TYPE_LOWER} STREQUAL "debug")
    # TODO: Consider to move debug symbols to file and then strip the library
    add_link_options(-s)
endif()

add_subdirectory_unique(vpu_driver/source)
add_subdirectory_unique(vpu_driver/unit_tests)
add_subdirectory_unique(level_zero_driver)
