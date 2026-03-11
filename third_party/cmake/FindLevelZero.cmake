# Copyright (C) 2019-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

find_package(PkgConfig)
if (NOT PkgConfig_FOUND)
    return()
endif()

set(MODULE_SPEC "level-zero")
if (LevelZero_FIND_VERSION_EXACT)
    set(MODULE_SPEC "${MODULE_SPEC}=${LevelZero_FIND_VERSION}")
elseif (LevelZero_FIND_VERSION)
    set(MODULE_SPEC "${MODULE_SPEC}>=${LevelZero_FIND_VERSION}")
endif()

pkg_check_modules(LevelZero IMPORTED_TARGET GLOBAL ${MODULE_SPEC})

if(NOT LevelZero_FOUND)
    return()
endif()

message(STATUS "LevelZero_INCLUDE_DIRS: ${LevelZero_INCLUDE_DIRS}")
message(STATUS "LevelZero_LIBRARIES:    ${LevelZero_LIBRARIES}")
message(STATUS "LevelZero_VERSION:      ${LevelZero_VERSION}")

add_library(ze_api_headers INTERFACE)
target_include_directories(ze_api_headers INTERFACE ${LevelZero_INCLUDE_DIRS})

add_library(ze_loader ALIAS PkgConfig::LevelZero)
