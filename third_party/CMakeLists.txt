#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(cmake/vpux_elf.cmake)
include(cmake/level-zero.cmake)
include(cmake/level-zero-npu-extensions.cmake)

set(INSTALL_GTEST OFF)
add_subdirectory(googletest EXCLUDE_FROM_ALL)

set(YAML_CPP_INSTALL OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)
