#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(SHARED_VPU_DEVICE_TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_device_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device_context_test.cpp
)

set_property(GLOBAL PROPERTY SHARED_VPU_DEVICE_TESTS ${SHARED_VPU_DEVICE_TESTS})

target_sources(${TARGET_NAME} PRIVATE
                ${SHARED_VPU_DEVICE_TESTS}
)
