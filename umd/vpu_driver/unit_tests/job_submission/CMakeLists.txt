#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(
  ${TARGET_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer_test.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/job_test.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/vpu_command_test.cpp)

add_subdirectories()
