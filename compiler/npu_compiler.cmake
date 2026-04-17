# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

add_library(npu_compiler INTERFACE)

if(NOT NPU_COMPILER_PACKAGE_DIR)
  if(ENABLE_NPU_COMPILER_BUILD)
    include(npu_compiler_build.cmake)
  else()
    message(WARNING "No NPU compiler provided, fallback to only include local copy of compiler headers")
    target_include_directories(npu_compiler INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    return()
  endif()
endif()

message(STATUS "NPU_COMPILER_PACKAGE_DIR: ${NPU_COMPILER_PACKAGE_DIR}")
target_include_directories(npu_compiler INTERFACE ${NPU_COMPILER_PACKAGE_DIR})

set(NPU_COMPILER_LIBS ${NPU_COMPILER_PACKAGE_DIR}/lib/libnpu_driver_compiler.so)

if(NOT ANDROID)
  # Set the RPATH for the npu_compiler to find the shared library at runtime
  target_link_libraries(npu_compiler INTERFACE ${NPU_COMPILER_LIBS})
endif()

install(FILES ${NPU_COMPILER_LIBS}
        TYPE LIB
        COMPONENT driver-compiler-npu)
