#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(test_app_lib)

# try to use c++17 or fallback to an older standard
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

include_directories(${CMAKE_BINARY_DIR})

add_library(${PROJECT_NAME} STATIC test_app.cpp perf_counter.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC gtest)
