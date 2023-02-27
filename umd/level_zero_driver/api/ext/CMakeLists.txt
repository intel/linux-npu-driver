#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(L0_SRCS_EXT_API
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_graph.cpp
)
set_property(GLOBAL APPEND PROPERTY L0_SRCS_EXT_API ${L0_SRCS_EXT_API})
