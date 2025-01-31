# Copyright 2022-2024 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

add_library(npu_compiler INTERFACE)

if(NOT NPU_COMPILER_PACKAGE_DIR)
  if(ENABLE_NPU_COMPILER_BUILD)
    include(npu_compiler_build.cmake)
  else()
    # TODO: Should be removed as driver should always be built with compiler
    message(WARNING "No NPU compiler provided, fallback to only include local copy of compiler headers")
    target_include_directories(npu_compiler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    return()
  endif()
endif()

message(STATUS "NPU_COMPILER_PACKAGE_DIR: ${NPU_COMPILER_PACKAGE_DIR}")
target_include_directories(npu_compiler INTERFACE ${NPU_COMPILER_PACKAGE_DIR})

set(NPU_COMPILER_LIBS ${NPU_COMPILER_PACKAGE_DIR}/lib/libnpu_driver_compiler.so)
target_link_libraries(npu_compiler INTERFACE ${NPU_COMPILER_LIBS})

install(FILES ${NPU_COMPILER_LIBS}
        TYPE LIB
        COMPONENT driver-compiler-npu)
