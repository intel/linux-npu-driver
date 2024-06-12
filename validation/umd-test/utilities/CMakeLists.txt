#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

add_library(data_handle OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/data_handle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/data_handle.cpp
)
target_compile_options(data_handle PRIVATE -Wno-strict-aliasing -Wno-uninitialized)
