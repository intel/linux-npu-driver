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

set(NPU_COMPILER_LIBS ${NPU_COMPILER_PACKAGE_DIR}/lib/libopenvino_intel_npu_compiler_loader.so
                      ${NPU_COMPILER_PACKAGE_DIR}/lib/libopenvino_intel_npu_compiler.so)

# Set the RPATH for the npu_compiler to find the shared library at runtime
target_link_libraries(npu_compiler INTERFACE ${NPU_COMPILER_LIBS})

# Bundle TBB only where the distribution does not provide it. On Ubuntu/RHEL
# libtbb.so.12 is owned by the system libtbb12 package, so shipping a private
# copy makes dpkg/rpm fail with a file conflict. There shlibdeps resolves the
# compiler's DT_NEEDED libtbb.so.12 into a dependency on the distro package.
set(NPU_COMPILER_INSTALL_PATTERNS PATTERN "libopenvino_intel_npu_compiler*.so*")
if(ANDROID)
  list(APPEND NPU_COMPILER_INSTALL_PATTERNS PATTERN "libtbb*.so*")
endif()

install(DIRECTORY ${NPU_COMPILER_PACKAGE_DIR}/lib/
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT driver-compiler-npu
        FILES_MATCHING ${NPU_COMPILER_INSTALL_PATTERNS})
