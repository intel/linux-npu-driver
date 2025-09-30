# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT

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
