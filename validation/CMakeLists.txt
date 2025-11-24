#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

if(NOT ENABLE_VALIDATION_BUILD)
    message(WARNING "The build does not include validation tests")
    return()
endif()

add_subdirectory(kmd-test)
add_subdirectory(test-app-lib)
add_subdirectory(umd-test)
