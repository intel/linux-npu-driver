#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/job.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/job.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fill_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fill_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inference_execute.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inference_execute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ts_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ts_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/copy_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/copy_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/barrier_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/barrier_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/query_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/query_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nop_command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nop_command.cpp
)
