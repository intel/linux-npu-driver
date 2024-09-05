#
# Copyright (C) 2024 Intel Corporation
#
# SPDX-License-Identifier: MIT


find_program(EBUILD_EXECUTABLE ebuild REQUIRED)

set(CPACK_GENERATOR External)
set(CPACK_SET_DESTDIR ON)
set(CPACK_EXTERNAL_ENABLE_STAGING ON)
set(CPACK_EXTERNAL_PACKAGE_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/ebuild_package_script.cmake)
