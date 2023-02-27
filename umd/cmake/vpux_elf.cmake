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

set(VPUX_ELF_DIR ${CMAKE_SOURCE_DIR}/umd/third_party/vpux_elf/vpux_elf)
set(VPUX_ELF_LOADER_INCLUDE_DIR ${VPUX_ELF_DIR}/loader/include)
set(VPUX_ELF_CORE_INCLUDE_DIR ${VPUX_ELF_DIR}/core/include)

include_directories(SYSTEM ${VPUX_ELF_CORE_INCLUDE_DIR})
include_directories(SYSTEM ${VPUX_ELF_LOADER_INCLUDE_DIR})

# Do not add target for second time to allow it to be included by other project then umd
if(TARGET vpux_elf)
  return()
endif()

# Below function is required by vpux_elf
function(enable_warnings_as_errors TARGET_NAME)
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror)
endfunction()

# TODO: Skip install(TARGETS) from vpux_elf
set(BUILD_SHARED_LIBS ON)
add_subdirectory(${VPUX_ELF_DIR})
unset(BUILD_SHARED_LIBS)

# TODO: Temporary WA for linker issue with missing -fPIC. This only occurs on Linux
set_property(TARGET vpux_elf PROPERTY POSITION_INDEPENDENT_CODE ON)
