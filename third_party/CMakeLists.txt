#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# Ignore compiler warnings from third party libraries
add_compile_options(-Wno-error -include cstdint)

include(cmake/vpux_elf.cmake)
include(cmake/level-zero.cmake)
include(cmake/level-zero-npu-extensions.cmake)
# TODO: Handle USE_SYSTEM_LIBRARIES for Perfetto SDK, too
if (ENABLE_NPU_PERFETTO_BUILD)
  if (USE_SYSTEM_LIBRARIES)
	  message(FATAL_ERROR "USE_SYSTEM_LIBRARIES is not yet implemented for the Perfetto SDK")
  endif()
  include(cmake/perfetto.cmake)
endif()

if (NOT USE_SYSTEM_LIBRARIES)
  set(INSTALL_GTEST OFF)
  add_subdirectory(googletest EXCLUDE_FROM_ALL)

  set(YAML_CPP_INSTALL OFF)
  set(BUILD_SHARED_LIBS OFF)
  add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)
else()
  message(STATUS "USE_SYSTEM_LIBRARIES excluded the vendored gtest/gmock and yaml-cpp from the build")
endif()
