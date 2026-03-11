# Copyright (C) 2022-2026 Intel Corporation
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
  # Enable macro security features
  $<$<CONFIG:Release>:-D_FORTIFY_SOURCE=2>
)

add_link_options(
  # Read only relocation (RERLO)
  "SHELL:$<$<CONFIG:Release>:-z relro>"
  # Immediate Binding protection
  "SHELL:$<$<CONFIG:Release>:-z now>"
  # Inexecutable stack
  "SHELL:$<$<CONFIG:Release>:-z noexecstack>"
)
