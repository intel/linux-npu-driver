#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME_L0} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/compiler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/compiler_common.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/compiler_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/elf_parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/elf_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/graph.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/disk_cache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/disk_cache.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/interface_parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/profiling_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/profiling_data.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/query_network.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/query_network.hpp
)
