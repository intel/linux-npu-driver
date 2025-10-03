# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT

# The libnpu_elf.a is picked up from compiler package, nothing to do
if (TARGET vpux_elf)
  return()
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
