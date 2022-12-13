#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(L0_SRCS_TOOLS_API
  ${CMAKE_CURRENT_SOURCE_DIR}/zet_metric.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_tools_loader.cpp
)
set_property(GLOBAL PROPERTY L0_SRCS_TOOLS_API ${L0_SRCS_TOOLS_API})
