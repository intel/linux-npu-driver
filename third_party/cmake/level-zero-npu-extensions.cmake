# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

set(LEVEL_ZERO_NPU_EXTENSIONS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/level-zero-npu-extensions)
target_include_directories(ze_api_headers SYSTEM INTERFACE ${LEVEL_ZERO_NPU_EXTENSIONS_INCLUDE_DIRS})

# FindLevelZero.cmake create an ALIASED_TARGET for ze_loader
get_target_property(ze_loader_aliased ze_loader ALIASED_TARGET)
if (ze_loader_aliased)
  target_include_directories(${ze_loader_aliased} INTERFACE ${LEVEL_ZERO_NPU_EXTENSIONS_INCLUDE_DIRS})
else()
  target_include_directories(ze_loader INTERFACE ${LEVEL_ZERO_NPU_EXTENSIONS_INCLUDE_DIRS})
endif()
