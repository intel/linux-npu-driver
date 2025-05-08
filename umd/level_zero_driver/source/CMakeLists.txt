#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME_L0_DRV level_zero_driver)

add_library(${TARGET_NAME_L0_DRV} STATIC)
set_property(TARGET ${TARGET_NAME_L0_DRV} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(${TARGET_NAME_L0_DRV}
    fw_vpu_api_headers
    vpu_driver
    vpux_elf
)

string(TIMESTAMP UNIX_SECONDS %s UTC)
set(DRIVER_VERSION ${UNIX_SECONDS} CACHE STRING "Decimal value that set Level Zero driverVersion")
message(STATUS "Level Zero driver version: ${DRIVER_VERSION}")

target_compile_definitions(${TARGET_NAME_L0_DRV} PUBLIC DRIVER_VERSION=${DRIVER_VERSION})

target_sources(${TARGET_NAME_L0_DRV} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/immediate_cmdlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/immediate_cmdlist.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdqueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver_handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver_handle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/eventpool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/eventpool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric_query.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric_query.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric_streamer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/metric_streamer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/compiler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/elf_parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/elf_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/graph.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/disk_cache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/disk_cache.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/interface_parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/profiling_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/profiling_data.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/query_network.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/query_network.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/sha1/sha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/sha1/sha1.h
)

target_include_directories(${TARGET_NAME_L0_DRV} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ext/sha1)
