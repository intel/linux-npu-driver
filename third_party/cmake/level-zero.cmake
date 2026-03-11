# Copyright (C) 2022-2026 Intel Corporation
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

  # Make ze_api_headers target to provide Level Zero headers to ze_loader and other targets
  add_library(ze_api_headers INTERFACE)
  target_include_directories(ze_api_headers SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/level-zero/include)

  # Link ze_loader with ze_api_headers to provide Level Zero headers
  target_link_libraries(ze_loader PUBLIC ze_api_headers)

  # EXCLUDE_FROM_ALL requires to add components from level-zero manually
  add_dependencies(ze_loader ze_validation_layer ze_tracing_layer)
  install(TARGETS ze_loader ze_validation_layer ze_tracing_layer
          COMPONENT level-zero)
endif()
