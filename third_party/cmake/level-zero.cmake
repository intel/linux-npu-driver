# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT

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
