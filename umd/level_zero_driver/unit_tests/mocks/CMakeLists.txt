#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_driver.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_metrics.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_metrics.cpp
)
