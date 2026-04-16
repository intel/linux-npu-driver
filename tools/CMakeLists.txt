#
# Copyright (C) 2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

if(NOT ENABLE_TOOLS_BUILD)
    message(WARNING "The build does not include utility tools")
    return()
endif()

add_subdirectory(intel-npu-smi)
