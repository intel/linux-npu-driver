#
# Copyright (C) 2022-2023 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(vpu-drv VERSION 0.2)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "CMAKE_BUILD_TYPE not specified, using Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: [Release, Debug]" FORCE)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
if (NOT ${BUILD_TYPE_LOWER} STREQUAL "debug" AND
    NOT ${BUILD_TYPE_LOWER} STREQUAL "release")
  message(FATAL_ERROR "Not supported CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}. "
                      "Supported types: [Release, Debug]")
endif()

message(STATUS "Using CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

if (NOT VPU_VERSION)
  set(VPU_VERSION "0.2")
endif()
configure_file(version.h.in ${CMAKE_BINARY_DIR}/version.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# VPUXCompilerL0
option(ENABLE_VPUX_COMPILER "Enable vpux_compiler" OFF)
message(STATUS "option ENABLE_VPUX_COMPILER: ${ENABLE_VPUX_COMPILER}")


# Add Intel CET flags for Indirect Branch Tracking and Shadow call-stack technologies.
add_compile_options(-fcf-protection=full -mshstk)

add_subdirectory(firmware)
add_subdirectory(umd)

# You may your custom commands in .local.cmake, i.e. driver compilation, running tests.
include(${CMAKE_CURRENT_SOURCE_DIR}/.local.cmake OPTIONAL)
