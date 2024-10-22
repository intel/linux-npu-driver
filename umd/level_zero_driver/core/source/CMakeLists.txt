#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

target_sources(${TARGET_NAME_L0_DRV} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist/cmdlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist/cmdlist.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist/immediate_cmdlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist/immediate_cmdlist.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdqueue/cmdqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdqueue/cmdqueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/context.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/context/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device/device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device/device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/driver_handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/driver_handle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/driver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/driver.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fence/fence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fence/fence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event/event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event/event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event/eventpool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event/eventpool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory/memory.cpp
)
