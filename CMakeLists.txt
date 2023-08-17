#
# Copyright (C) 2022-2023 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(vpu-drv VERSION 0.3)

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
  set(VPU_VERSION "0.3")
endif()
configure_file(version.h.in ${CMAKE_BINARY_DIR}/version.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_OPENVINO_PACKAGE "Enable building the OpenVINO + VPUX Plugin package" OFF)
option(ENABLE_VPUX_COMPILER "Enable vpux_compiler" OFF)

message(STATUS "option ENABLE_OPENVINO_PACKAGE: ${ENABLE_OPENVINO_PACKAGE}")
message(STATUS "option ENABLE_VPUX_COMPILER: ${ENABLE_VPUX_COMPILER}")


# Add Intel CET flags for Indirect Branch Tracking and Shadow call-stack technologies.
add_compile_options(-fcf-protection=full -mshstk)

add_subdirectory(compiler)
add_subdirectory(firmware)
add_subdirectory(third_party)
add_subdirectory(umd)

# Add options for building with sanitizers
if(NOT ENABLE_UMD_SANITIZER)
  message(STATUS "ENABLE_UMD_SANITIZER not specified, using no sanitizers")
  set(ENABLE_UMD_SANITIZER "" CACHE STRING "Enable UMD vpu test and unit test thread sanitizer. Set empty string to disable sanitizer. [address,undefined | thread | memory]" FORCE)
endif()
if (ENABLE_UMD_SANITIZER STREQUAL "memory" AND CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  message(FATAL_ERROR "GCC doesn't support memory sanitizer.")
endif()
message(STATUS "option ENABLE_UMD_SANITIZER: ${ENABLE_UMD_SANITIZER}")

# You may your custom commands in .local.cmake, i.e. driver compilation, running tests.
include(${CMAKE_CURRENT_SOURCE_DIR}/.local.cmake OPTIONAL)
