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

# Add options for building with sanitizers
if(NOT ENABLE_SANITIZER)
  message(STATUS "ENABLE_SANITIZER not specified, using no sanitizers")
endif()
if (ENABLE_SANITIZER STREQUAL "memory" AND CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  message(FATAL_ERROR "GCC doesn't support memory sanitizer.")
endif()
message(STATUS "option ENABLE_SANITIZER: ${ENABLE_SANITIZER}")

if (ENABLE_SANITIZER STREQUAL "address,undefined")
  add_compile_options(-fsanitize=address,undefined)
  link_libraries(-fsanitize=address,undefined)
  if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_link_options(-static-libasan -static-libubsan)
  endif()
elseif (ENABLE_SANITIZER STREQUAL "thread")
  add_compile_options(-fsanitize=thread)
  link_libraries(-fsanitize=thread)
  if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_link_options(-static-libtsan)
  endif()
elseif (ENABLE_SANITIZER STREQUAL "memory")
  add_compile_options(-fsanitize=memory)
  link_libraries(-fsanitize=memory)
endif()
if (ENABLE_SANITIZER AND CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  add_link_options(-static-libsan)
endif()
