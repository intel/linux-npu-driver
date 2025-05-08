# Copyright 2024 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.


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
