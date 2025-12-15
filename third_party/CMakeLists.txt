#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# Ignore compiler warnings from third party libraries
add_compile_options(-Wno-error -include cstdint)

include(cmake/npu_compiler_elf.cmake)
include(cmake/level-zero.cmake)
include(cmake/level-zero-npu-extensions.cmake)
if (ENABLE_NPU_PERFETTO_BUILD)
  include(cmake/perfetto.cmake)
endif()

set(INSTALL_GTEST OFF)
add_subdirectory(googletest EXCLUDE_FROM_ALL)

set(YAML_CPP_INSTALL OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)
