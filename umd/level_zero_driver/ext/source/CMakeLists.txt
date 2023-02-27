#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(L0_EXT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/compiler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/elf_parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/elf_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/graph.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/profiling_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/profiling_data.hpp
)

target_include_directories(${TARGET_NAME_L0} PUBLIC graph)

if(NOT DEFINED L0_DRIVER_VERSION)
  set(L0_DRIVER_VERSION 1)
endif()

# Make our source files visible to parent
set_property(GLOBAL PROPERTY L0_EXT_SOURCE ${L0_EXT_SOURCE})
