#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

add_subdirectory(core)
add_subdirectory(ext)
add_subdirectory(tools)

set(L0_API_SOURCE "")
set_property(GLOBAL PROPERTY L0_API_SOURCE ${L0_API_SOURCE})
