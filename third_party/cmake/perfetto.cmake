# Copyright (C) 2024-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT

project(perfetto)
find_package(Threads)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(perfetto STATIC "${CMAKE_CURRENT_SOURCE_DIR}/perfetto/sdk/perfetto.cc")
target_include_directories(perfetto INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/perfetto/sdk")
if (ANDROID)
    # perfetto requires liblog.so on Android build
    target_link_libraries(perfetto INTERFACE log)
endif()

set_property(TARGET perfetto PROPERTY POSITION_INDEPENDENT_CODE ON)
