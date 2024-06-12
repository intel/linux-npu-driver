#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(test_app_lib)

# try to use c++17 or fallback to an older standard
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_BINARY_DIR})
include_directories(../../linux/include)

add_library(${PROJECT_NAME} STATIC test_app.cpp drm_helpers.cpp perf_counter.cpp file_helper.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC gtest)
