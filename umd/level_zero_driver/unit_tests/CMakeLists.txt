#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# use SKIP_UNIT_TESTS=ON to skip building unit tests
if(SKIP_UNIT_TESTS)
  message(STATUS "Skip building unit tests")
  return()
endif()

set(TARGET_NAME ze_intel_vpu_tests)

add_executable(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(
  ${TARGET_NAME}
  level_zero_driver
  fw_vpu_api_headers
  gmock
  gtest
  vpu_driver_mocks)

add_subdirectories()
