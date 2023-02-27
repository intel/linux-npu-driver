#
# Copyright (C) 2023 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

project(fw_vpu_api_headers)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)
