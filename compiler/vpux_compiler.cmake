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

add_library(vpux_compiler INTERFACE)

if(ENABLE_VPUX_COMPILER_BUILD)
  include(vpux_compiler_build.cmake)

  add_dependencies(vpux_compiler ${VPUX_COMPILER_DEPENDENCY})
  install(FILES ${VPUX_COMPILER_LIBS}
          TYPE LIB
          COMPONENT driver-compiler-npu)
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/)
  set(VPUX_COMPILER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
else()
  message(FATAL_ERROR "Missing VPUXCompilerL0 headers in path ${CMAKE_CURRENT_SOURCE_DIR}/include/")
endif()

target_include_directories(vpux_compiler INTERFACE ${VPUX_COMPILER_INCLUDE_DIR})
