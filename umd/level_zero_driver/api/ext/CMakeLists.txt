#
# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME_L0} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_graph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_queue.cpp
)
