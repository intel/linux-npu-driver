#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

macro(hide_subdir subdir)
  file(RELATIVE_PATH subdir_relative ${VPU_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
  set(${subdir_relative}_hidden} TRUE)
endmacro()

macro(add_subdirectory_unique subdir)
  file(RELATIVE_PATH subdir_relative ${VPU_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
  if(NOT ${subdir_relative}_hidden})
    add_subdirectory(${subdir} ${ARGN})
  endif()
  hide_subdir(${subdir})
endmacro()

macro(add_subdirectories)
  file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
  foreach(subdir ${subdirectories})
    file(RELATIVE_PATH subdir_relative ${VPU_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt AND NOT ${subdir_relative}_hidden})
      add_subdirectory(${subdir})
    endif()
  endforeach()
endmacro()

macro(append_sources_from_properties list_name)
  foreach(name ${ARGN})
    get_property(${name} GLOBAL PROPERTY ${name})
    list(APPEND ${list_name} ${${name}})
  endforeach()
endmacro()
