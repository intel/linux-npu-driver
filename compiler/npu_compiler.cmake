# Copyright 2022-2023 Intel Corporation.
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

if(ENABLE_NPU_COMPILER_BUILD)
  include(npu_compiler_build.cmake)

  add_dependencies(npu_compiler ${NPU_COMPILER_DEPENDENCY})
  install(FILES ${NPU_COMPILER_LIBS}
          TYPE LIB
          COMPONENT driver-compiler-npu)
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/)
  set(NPU_COMPILER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
else()
  message(FATAL_ERROR "Missing NPU Driver Compiler headers in path ${CMAKE_CURRENT_SOURCE_DIR}/include/")
endif()

target_include_directories(npu_compiler INTERFACE ${NPU_COMPILER_INCLUDE_DIR})
