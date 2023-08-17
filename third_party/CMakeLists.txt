#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(cmake/vpux_elf.cmake)
include(cmake/level-zero.cmake)
include(cmake/level-zero-vpu-extensions.cmake)

set(INSTALL_GTEST OFF)
add_subdirectory(googletest EXCLUDE_FROM_ALL)
