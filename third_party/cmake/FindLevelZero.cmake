#
# Copyright (C) 2019-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

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

pkg_check_modules(LevelZero ${MODULE_SPEC})

if(NOT LevelZero_FOUND)
    return()
endif()

list(APPEND LevelZero_LIBRARIES ${CMAKE_DL_LIBS})

if(NOT TARGET LevelZero::LevelZero)
    add_library(LevelZero::LevelZero INTERFACE IMPORTED)
    set_target_properties(LevelZero::LevelZero
      PROPERTIES INTERFACE_LINK_LIBRARIES "${LevelZero_LIBRARIES}"
    )
    set_target_properties(LevelZero::LevelZero
      PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${LevelZero_INCLUDE_DIRS}"
    )
endif()

list(REMOVE_ITEM LevelZero_INCLUDE_DIRS "/usr/include")

message(STATUS "LevelZero_LIBRARIES: " ${LevelZero_LIBRARIES})
message(STATUS "LevelZero_INCLUDE_DIRS: " ${LevelZero_INCLUDE_DIRS})
message(STATUS "LevelZero_VERSION: " ${LevelZero_VERSION})
