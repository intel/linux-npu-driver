#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# use SKIP_UNIT_TESTS=ON to skip building unit tests
if(SKIP_UNIT_TESTS)
  message(STATUS "Skip building unit tests")
  return()
endif()

# build umd unit tests with sanitizers
umd_sanitizer_flags()

set(TARGET_NAME ${TARGET_NAME_L0}_tests)

add_executable(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(
  ${TARGET_NAME}
  ${TARGET_NAME_L0}
  fw_vpu_api_headers
  gmock
  gtest
  vpu_driver_mocks)

add_subdirectories()
