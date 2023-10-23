#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# use SKIP_UNIT_TESTS=ON to skip building unit tests
if(SKIP_UNIT_TESTS)
  message(STATUS "Skip building VPU unit tests")
  return()
endif()

set(TARGET_NAME vpu_shared_tests)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} vpu_driver gtest gmock vpu_driver_mocks)

add_subdirectories()
