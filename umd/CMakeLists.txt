#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(common_macros.cmake)
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)

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

# Project-wide include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver/include")
include_directories(${VPU_SOURCE_DIR})
include_directories(SYSTEM ${VPU_KMD_INCLUDE_DIR})

add_subdirectory_unique(vpu_driver/source)
add_subdirectory_unique(vpu_driver/unit_tests)
add_subdirectory_unique(level_zero_driver)
