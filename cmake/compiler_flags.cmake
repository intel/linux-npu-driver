# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT

add_compile_options(
  # Compiler warnings
  -Wall -Wextra -Werror -Wno-unused-parameter -Wvla -Wno-error=cpp
  # Control flow integrity
  $<$<CONFIG:Release>:-fcf-protection=full>
  # Format string defence
  -Wformat -Wformat-security -Werror=format-security
  # Stack protection
  $<$<CONFIG:Debug>:-fstack-protector>
  $<$<CONFIG:Release>:-fstack-protector-strong>
  $<$<CONFIG:Release>:-fstack-clash-protection>
  # Use relative paths for debug information
  -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.
)

if(NOT ${LINUX_SYSTEM_NAME} STREQUAL "cros_sdk")
    add_compile_options(
      # Enable macro security features
      $<$<CONFIG:Release>:-D_FORTIFY_SOURCE=2>
    )
endif()

add_link_options(
  # Read only relocation (RERLO)
  "SHELL:$<$<CONFIG:Release>:-z relro>"
  # Immediate Binding protection
  "SHELL:$<$<CONFIG:Release>:-z now>"
  # Inexecutable stack
  "SHELL:$<$<CONFIG:Release>:-z noexecstack>"
)
