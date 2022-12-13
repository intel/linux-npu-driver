#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME ${TARGET_NAME_L0}_mocks)

set(L0_MOCKS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/mock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_driver.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_metrics.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock_metrics.cpp
)

add_library(${TARGET_NAME} OBJECT ${L0_MOCKS_SOURCES})
