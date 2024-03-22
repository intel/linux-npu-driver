#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/test_graph.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/test_graph_cid.cpp
)
