#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME vpu_driver)

add_subdirectories()

append_sources_from_properties(CORE_SOURCES
                               VPU_CORE_DEVICE
                               VPU_CORE_MEMORY
                               VPU_CORE_OS_INTERFACE
                               VPU_CORE_UTILITIES
                               VPU_COMMANDS
)

add_library(${TARGET_NAME} STATIC ${CORE_SOURCES})
set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
