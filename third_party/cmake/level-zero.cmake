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
  set(LEVEL_ZERO_BINARY_DIR ${CMAKE_BINARY_DIR}/third_party/level-zero/build)

  include(ExternalProject)
  ExternalProject_Add(
    level-zero
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/level-zero
    BINARY_DIR ${LEVEL_ZERO_BINARY_DIR}
    INSTALL_COMMAND
      cp -a ${LEVEL_ZERO_BINARY_DIR}/lib/libze_loader.so
      ${LEVEL_ZERO_BINARY_DIR}/lib/libze_loader.so.1
      ${LEVEL_ZERO_BINARY_DIR}/lib/libze_loader.so.1.10.0
      ${LEVEL_ZERO_BINARY_DIR}/lib/libze_validation_layer.so
      ${LEVEL_ZERO_BINARY_DIR}/lib/libze_validation_layer.so.1
      ${LEVEL_ZERO_BINARY_DIR}/lib/libze_validation_layer.so.1.10.0
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
  BUILD_BYPRODUCTS
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_loader.so.1
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_validation_layer.so.1
      )
  add_library(ze_loader SHARED IMPORTED GLOBAL)
  add_dependencies(ze_loader level-zero)
  set_target_properties(
    ze_loader PROPERTIES IMPORTED_LOCATION
                         ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_loader.so.1)

  install(
    FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_loader.so
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_loader.so.1
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_loader.so.1.10.0
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_validation_layer.so
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_validation_layer.so.1
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libze_validation_layer.so.1.10.0
    TYPE LIB
    COMPONENT level-zero)

  set(LevelZero_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/level-zero/include)
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
