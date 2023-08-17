# Copyright 2022-2023 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(LevelZero)
if(NOT LevelZero_FOUND)
  message(STATUS "LevelZero not found in the system, take one from third_party/level_zero")

  set(LevelZero_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/level-zero/include)
  add_subdirectory(level-zero EXCLUDE_FROM_ALL)
  add_dependencies(ze_loader ze_validation_layer)
  install(TARGETS ze_loader ze_validation_layer)
else()
  set(LevelZero_INCLUDE_DIR ${LevelZero_INCLUDE_DIR}/level_zero)
endif()

# TODO: Get rid of copying the headers if level-zero is installed
set(LEVEL_ZERO_HEADERS_DIR "${CMAKE_BINARY_DIR}/include/level_zero")
file(MAKE_DIRECTORY ${LEVEL_ZERO_HEADERS_DIR})
file(GLOB LEVEL_ZERO_HEADERS
  ${LevelZero_INCLUDE_DIR}/*.h
  ${LevelZero_INCLUDE_DIR}/layers
  ${LevelZero_INCLUDE_DIR}/loader)
file(COPY ${LEVEL_ZERO_HEADERS} DESTINATION ${LEVEL_ZERO_HEADERS_DIR})
