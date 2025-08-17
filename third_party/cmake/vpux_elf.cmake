# Copyright 2022-2024 Intel Corporation.
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

# The libnpu_elf.a is picked up from compiler package, nothing to do
if (TARGET vpux_elf)
  return()
endif()

# TODO: Detect empty directory and upgrade to warning then regardless
if (USE_SYSTEM_LIBRARIES)
  message(STATUS "USE_SYSTEM_LIBRARIES does not apply to vpux_elf; it must always be vendored.")
endif()

# Below function is required by vpux_elf
function(enable_warnings_as_errors TARGET_NAME)
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror)
endfunction()

get_target_property(FIRMWARE_INCLUDES fw_vpu_api_headers INTERFACE_INCLUDE_DIRECTORIES)
include_directories(SYSTEM ${FIRMWARE_INCLUDES})

# TODO: Skip install(TARGETS) from vpux_elf
set(BUILD_SHARED_LIBS ON)
add_subdirectory(vpux_elf/vpux_elf)
unset(BUILD_SHARED_LIBS)

# TODO: Temporary WA for linker issue with missing -fPIC. This only occurs on Linux
set_property(TARGET vpux_elf PROPERTY POSITION_INDEPENDENT_CODE ON)
