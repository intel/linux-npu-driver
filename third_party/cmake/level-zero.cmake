# Copyright 2022-2024 Intel Corporation.
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

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/level-zero/.git)
    execute_process(COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR}/level-zero name-rev --tags --name-only HEAD
                    OUTPUT_VARIABLE LEVEL_ZERO_TAG
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.?[0-9]*$" LEVEL_ZERO_VERSION ${LEVEL_ZERO_TAG})
endif()

find_package(LevelZero ${LEVEL_ZERO_VERSION})

if(NOT LevelZero_FOUND)
  message(STATUS "LevelZero not found in the system, take one from third_party/level_zero")

  # EXCLUDE_FROM_ALL is used because level-zero-devel install destination starts with root
  add_subdirectory(level-zero EXCLUDE_FROM_ALL)

  # EXCLUDE_FROM_ALL requires to add components from level-zero manually
  add_dependencies(ze_loader ze_validation_layer ze_tracing_layer)
  install(TARGETS ze_loader ze_validation_layer ze_tracing_layer
          COMPONENT level-zero)

  set(LevelZero_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/level-zero/include)
endif()

# TODO: Get rid of copying the headers if level-zero is installed
set(LEVEL_ZERO_HEADERS_DIR "${CMAKE_BINARY_DIR}/include/level_zero")
file(MAKE_DIRECTORY ${LEVEL_ZERO_HEADERS_DIR})
foreach(LevelZero_INCLUDE_DIR IN LISTS LevelZero_INCLUDE_DIRS)
    file(GLOB LEVEL_ZERO_HEADERS
        ${LevelZero_INCLUDE_DIR}/*.h
        ${LevelZero_INCLUDE_DIR}/layers
        ${LevelZero_INCLUDE_DIR}/loader)
    file(COPY ${LEVEL_ZERO_HEADERS} DESTINATION ${LEVEL_ZERO_HEADERS_DIR})
endforeach()
