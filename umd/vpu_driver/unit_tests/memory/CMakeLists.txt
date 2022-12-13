#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_MEMORY_TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_object_test.cpp
)

set_property(GLOBAL PROPERTY VPU_MEMORY_TESTS ${VPU_MEMORY_TESTS})

target_sources(${TARGET_NAME} PRIVATE
                ${VPU_MEMORY_TESTS}
)
