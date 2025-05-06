#
# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/log.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stats.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stats.cpp
)

if (ENABLE_NPU_PERFETTO_BUILD)
  target_sources(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/trace_perfetto.cpp)
  target_link_libraries(${TARGET_NAME} perfetto)
  target_compile_definitions(${TARGET_NAME} PUBLIC ENABLE_NPU_PERFETTO_BUILD=1)
endif()
