#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

add_subdirectory(core)
add_subdirectory(tools)

set(L0_API_SOURCE "")
append_sources_from_properties(L0_API_SOURCE
  L0_SRCS_CORE_API
  L0_SRCS_EXT_API
  L0_SRCS_TOOLS_API)
set_property(GLOBAL PROPERTY L0_API_SOURCE ${L0_API_SOURCE})
