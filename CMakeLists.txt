#
# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
    cmake_policy(SET CMP0135 NEW)
endif()

set(STACK_VERSION 1.38.0 CACHE STRING "Main project version")
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

set(ENABLE_OPENVINO_PACKAGE_DEFAULT OFF)
set(ENABLE_NPU_COMPILER_BUILD_DEFAULT OFF)
set(ENABLE_NPU_PERFETTO_BUILD_DEFAULT OFF)
set(ENABLE_VALIDATION_BUILD_DEFAULT ON)
set(ENABLE_OFFLINE_COMPILATION_SUPPORT_DEFAULT OFF)
set(ENABLE_COMPILATION_FLAGS_OVERRIDE_DEFAULT OFF)
# By default, the npu_elf target is picked up from the compiler package
set(ENABLE_NPU_ELF_BUILD_DEFAULT OFF)
set(ENABLE_TOOLS_BUILD_DEFAULT OFF)
set(ENABLE_NPU_ALT_DEPENDENCY_PATH_OVERRIDE_DEFAULT OFF)

option(ENABLE_OPENVINO_PACKAGE "Enable building the OpenVINO package" ${ENABLE_OPENVINO_PACKAGE_DEFAULT})
option(ENABLE_NPU_COMPILER_BUILD "Enable building compiler in driver" ${ENABLE_NPU_COMPILER_BUILD_DEFAULT})
option(ENABLE_NPU_PERFETTO_BUILD "Enable building with Perfetto library" ${ENABLE_NPU_PERFETTO_BUILD_DEFAULT})
option(ENABLE_VALIDATION_BUILD "Enable building validation tests" ${ENABLE_VALIDATION_BUILD_DEFAULT})
option(ENABLE_OFFLINE_COMPILATION_SUPPORT "Enable model compilation support for user defined device" ${ENABLE_OFFLINE_COMPILATION_SUPPORT_DEFAULT})
option(ENABLE_COMPILATION_FLAGS_OVERRIDE "Allow to override flags passed to compiler" ${ENABLE_COMPILATION_FLAGS_OVERRIDE_DEFAULT})
option(ENABLE_NPU_ELF_BUILD "Enable building NPU Elf library in driver" ${ENABLE_NPU_ELF_BUILD_DEFAULT})
option(ENABLE_TOOLS_BUILD "Enable building tools" ${ENABLE_TOOLS_BUILD_DEFAULT})
option(ENABLE_NPU_ALT_DEPENDENCY_PATH_OVERRIDE "Allow to override path for dependencies in driver" ${ENABLE_NPU_ALT_DEPENDENCY_PATH_OVERRIDE_DEFAULT})

message(STATUS "option ENABLE_OPENVINO_PACKAGE: ${ENABLE_OPENVINO_PACKAGE}")
message(STATUS "option ENABLE_NPU_COMPILER_BUILD: ${ENABLE_NPU_COMPILER_BUILD}")
message(STATUS "option ENABLE_NPU_PERFETTO_BUILD: ${ENABLE_NPU_PERFETTO_BUILD}")
message(STATUS "option ENABLE_VALIDATION_BUILD: ${ENABLE_VALIDATION_BUILD}")
message(STATUS "option ENABLE_OFFLINE_COMPILATION_SUPPORT: ${ENABLE_OFFLINE_COMPILATION_SUPPORT}")
message(STATUS "option ENABLE_COMPILATION_FLAGS_OVERRIDE: ${ENABLE_COMPILATION_FLAGS_OVERRIDE}")
message(STATUS "option ENABLE_NPU_ELF_BUILD: ${ENABLE_NPU_ELF_BUILD}")
message(STATUS "option ENABLE_TOOLS_BUILD: ${ENABLE_TOOLS_BUILD}")
message(STATUS "option ENABLE_NPU_ALT_DEPENDENCY_PATH_OVERRIDE: ${ENABLE_NPU_ALT_DEPENDENCY_PATH_OVERRIDE}")

include(GNUInstallDirs)

include(cmake/android_libc++_shared.cmake)
include(cmake/detect_linux_system.cmake)
include(cmake/compiler_flags.cmake)
include(cmake/sanitizer.cmake)

set(VALIDATION_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}/vpu/validation")
set(OPENVINO_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/vpu/openvino")

add_subdirectory(compiler)
add_subdirectory(firmware)
add_subdirectory(third_party)
add_subdirectory(tools)
add_subdirectory(umd)
add_subdirectory(validation)

include(cmake/packaging/setup.cmake)

# You may your custom commands in .local.cmake, i.e. driver compilation, running tests.
include(${CMAKE_CURRENT_SOURCE_DIR}/.local.cmake OPTIONAL)
