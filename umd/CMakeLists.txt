#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)

project(ivpudrv)

include(common_macros.cmake)
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)

include(cmake/vpux_compiler.cmake)
include(cmake/vpux_elf.cmake)
include(cmake/utilities.cmake)

# Check if boost is present before compiling
find_package(Boost 1.71.0)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Boost headers can't be found, or installed version < 1.71.")
endif()

# Logging is available in debug builds only.
if(${BUILD_TYPE_LOWER} STREQUAL "debug")
  string(TOUPPER "${UMD_LOGLEVEL}" UMD_LOGLEVEL)

  # Default log level is ERROR.
  if(NOT UMD_LOGLEVEL)
    set(UMD_LOGLEVEL "ERROR")
  endif()

  # Check user passing log level.
  if(NOT UMD_LOGLEVEL STREQUAL "VERBOSE"
      AND NOT UMD_LOGLEVEL STREQUAL "INFO"
      AND NOT UMD_LOGLEVEL STREQUAL "WARNING"
      AND NOT UMD_LOGLEVEL STREQUAL "ERROR"
      AND NOT UMD_LOGLEVEL STREQUAL "QUIET")
      # Unsupported log level.
      message(FATAL_ERROR "Unsupported log level (verbose, info, warning, error, quiet)")
  endif()

  # Pass the requested log level to CXX_FLAGS.
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUMD_LOGLEVEL=${UMD_LOGLEVEL}")
  message(STATUS "UMD log level: ${UMD_LOGLEVEL}")
else()
  # Logging is for debugging builds only.
  if(UMD_LOGLEVEL)
    message(WARNING "UMD_LOGLEVEL is ignored for Debug build")
  endif()
endif()

# Set our build directory
if(NOT DEFINED VPU_BUILD_DIR)
  set(VPU_BUILD_DIR ${CMAKE_BINARY_DIR})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT VPU_KMD_INCLUDE_DIR)
  set(VPU_KMD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../linux/include")
endif()

message(STATUS "KMD include dir: ${VPU_KMD_INCLUDE_DIR}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Set the runtime source directory
if(NOT DEFINED VPU_SOURCE_DIR)
  set(VPU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(GTEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/gtest")

# Project-wide include paths
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vpu_driver/include")
include_directories(${VPU_BUILD_DIR})
include_directories(${VPU_SOURCE_DIR})
include_directories(SYSTEM ${VPU_KMD_INCLUDE_DIR})
include_directories(SYSTEM ${VPU_FW_INCLUDE_DIR})
include_directories(SYSTEM ${GTEST_INCLUDE_DIR})
include_directories(SYSTEM ${VPUX_COMPILER_INCLUDE_DIR})

# Build the third party modules with the default compiler switches.
add_subdirectory_unique(third_party/level-zero EXCLUDE_FROM_ALL)

add_dependencies(ze_loader ze_validation_layer)

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

install(TARGETS ze_intel_vpu ze_loader ze_validation_layer)
