#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
    cmake_policy(SET CMP0135 NEW)
endif()

set(STACK_VERSION 1.8.0 CACHE STRING "Main project version")
project(npu-linux-driver VERSION ${STACK_VERSION})

set(BUILD_NUMBER "dev-0" CACHE STRING "Build number composed of name and unique number used as driver version")
set(BUILD_VERSION ${PROJECT_NAME}-dev-${PROJECT_VERSION}.${BUILD_NUMBER} CACHE STRING "Build version")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Supported build types: Release, Debug" FORCE)
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)

message(STATUS "PROJECT_VERSION = ${PROJECT_VERSION}")
message(STATUS "BUILD_NUMBER = ${BUILD_NUMBER}")
message(STATUS "BUILD_VERSION = ${BUILD_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

configure_file(version.h.in ${CMAKE_BINARY_DIR}/include/version.h)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_OPENVINO_PACKAGE "Enable building the OpenVINO + NPU Plugin package" OFF)
option(ENABLE_NPU_COMPILER_BUILD "Enable building compiler in driver" OFF)

message(STATUS "option ENABLE_OPENVINO_PACKAGE: ${ENABLE_OPENVINO_PACKAGE}")
message(STATUS "option ENABLE_NPU_COMPILER_BUILD: ${ENABLE_NPU_COMPILER_BUILD}")

include(GNUInstallDirs)

include(cmake/detect_linux_system.cmake)
include(cmake/compiler_flags.cmake)
include(cmake/sanitizer.cmake)

add_subdirectory(compiler)
add_subdirectory(firmware)
add_subdirectory(third_party)
add_subdirectory(umd)
add_subdirectory(validation)

include(cmake/packaging/setup.cmake)

# You may your custom commands in .local.cmake, i.e. driver compilation, running tests.
include(${CMAKE_CURRENT_SOURCE_DIR}/.local.cmake OPTIONAL)
