#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(L0_CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist/cmdlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmdlist/cmdlist.hpp
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

if(NOT DEFINED L0_DRIVER_VERSION)
  set(L0_DRIVER_VERSION 1)
endif()

# Put Driver version into define
configure_file(driver/driver_version.h.in ${CMAKE_BINARY_DIR}/driver_version_l0.h)

# Make our source files visible to parent
set_property(GLOBAL PROPERTY L0_CORE_SOURCE ${L0_CORE_SOURCE})
