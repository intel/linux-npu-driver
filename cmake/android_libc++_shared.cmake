# Copyright (C) 2026 Intel Corporation
#
# SPDX-License-Identifier: MIT


if(ANDROID AND ANDROID_STL STREQUAL "c++_shared")
  find_library(
    ANDROID_CXX_SHARED_LIBRARY
    NAMES c++_shared
    REQUIRED)

  message(STATUS "Android libc++_shared.so: ${ANDROID_CXX_SHARED_LIBRARY}")

  add_custom_target(
    android_runtime_libcxx ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${ANDROID_CXX_SHARED_LIBRARY}"
          "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libc++_shared.so"
    VERBATIM)

  install(
    FILES "${ANDROID_CXX_SHARED_LIBRARY}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT level-zero-npu)
endif()
