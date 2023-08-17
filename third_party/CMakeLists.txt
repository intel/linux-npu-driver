#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(cmake/vpux_elf.cmake)
include(cmake/level-zero.cmake)
include(cmake/level-zero-vpu-extensions.cmake)

add_subdirectory(googletest EXCLUDE_FROM_ALL)
