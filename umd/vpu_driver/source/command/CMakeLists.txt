#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_command_buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_command_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_job.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_job.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_event_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_event_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_ts_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_ts_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_copy_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_copy_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_memory_fill_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_memory_fill_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_graph_init_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_graph_init_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_graph_exe_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_graph_exe_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_barrier_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_barrier_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_query_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_query_command.cpp
)
