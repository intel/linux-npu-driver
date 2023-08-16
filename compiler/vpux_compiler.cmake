# Copyright 2022-2023 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this
# software or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express
# or implied warranties, other than those that are expressly stated in
# the License.

if(NOT ENABLE_VPUX_COMPILER)
  return()
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vpux_compiler_cache.cmake AND ENABLE_VPUX_COMPILER_CACHE)
  include(vpux_compiler_cache.cmake)
else()
  include(vpux_compiler_build.cmake)
endif()

add_library(vpux_compiler SHARED IMPORTED GLOBAL)
add_dependencies(vpux_compiler ${VPUX_COMPILER_DEPENDENCY})

set_target_properties(vpux_compiler PROPERTIES
  IMPORTED_LOCATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libVPUXCompilerL0.so
  IMPORTED_LINK_DEPENDENT_LIBRARIES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtbb.so.12)

target_include_directories(vpux_compiler INTERFACE ${VPUX_COMPILER_INCLUDE_DIR})
target_compile_definitions(vpux_compiler INTERFACE ENABLE_VPUX_COMPILER)

install(FILES
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libVPUXCompilerL0.so
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtbb.so.12
  TYPE LIB)
