#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_buffer_object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vpu_buffer_object.hpp
)
