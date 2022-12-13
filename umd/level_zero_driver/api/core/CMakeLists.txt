#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(L0_SRCS_CORE_API
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_barrier.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_cmdlist.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_cmdqueue.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_context.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_core_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_driver.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_device.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_fence.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_event.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_memory.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ze_copy.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/zes_loader.cpp
)
set_property(GLOBAL PROPERTY L0_SRCS_CORE_API ${L0_SRCS_CORE_API})
