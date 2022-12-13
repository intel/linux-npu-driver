#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(VPU_JOB_SUBMISSION_TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine_group_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/job_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_command_test.cpp
)

set_property(GLOBAL PROPERTY VPU_JOB_SUBMISSION_TESTS ${VPU_JOB_SUBMISSION_TESTS})

target_sources(${TARGET_NAME} PRIVATE
                ${VPU_JOB_SUBMISSION_TESTS}
)
