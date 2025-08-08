#
# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# use SKIP_UNIT_TESTS=ON to skip building unit tests
if(SKIP_UNIT_TESTS)
  message(STATUS "Skip building VPU unit tests")
  return()
endif()

set(TARGET_NAME npu_shared_tests)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} vpu_driver gtest gmock vpu_driver_mocks
  # Required by VPUInferenceExecute that use ElfParser from level_zero_driver
  level_zero_driver)

add_subdirectories()
