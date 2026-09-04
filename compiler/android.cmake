# Copyright (C) 2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

if(NOT ANDROID)
  message(FATAL_ERROR "This CMake script is intended to be used for Android builds only.")
endif()

# Collect Android specific CMake cache variables
get_cmake_property(cache_vars CACHE_VARIABLES)
list(FILTER cache_vars INCLUDE REGEX "^ANDROID_")
foreach(cache_var ${cache_vars})
  message(STATUS "Android specific cache variables: -D${cache_var}=${${cache_var}}")
  list(APPEND ANDROID_CMAKE_ARGS -D${cache_var}='${${cache_var}}')
endforeach()

# Configure TBB if selected
if(THREADING STREQUAL "TBB")
  message(STATUS "Configuring NPU Compiler build to use TBB threading")
  set(ONETBB_BUILD onetbb_build)
  set(ONETBB_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/onetbb-install)

  ExternalProject_Add(
    ${ONETBB_BUILD}
    GIT_REPOSITORY https://github.com/oneapi-src/oneTBB
    GIT_TAG v2023.0.0
    GIT_SHALLOW TRUE
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/onetbb
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/onetbb-build
    INSTALL_DIR ${ONETBB_INSTALL_DIR}
    CMAKE_ARGS
      ${ANDROID_CMAKE_ARGS}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
      -DCMAKE_INSTALL_PREFIX=${ONETBB_INSTALL_DIR}
      -DCMAKE_BUILD_TYPE=Release
      -DTBB_TEST=OFF
      -DTBB_STRICT=OFF
      -DCMAKE_SHARED_LINKER_FLAGS=-Wl,--undefined-version
  )

  list(APPEND TBB_CMAKE_ARGS "-DTBBROOT=${ONETBB_INSTALL_DIR}")
  list(APPEND TBB_CMAKE_ARGS "-DTBB_DIR=${ONETBB_INSTALL_DIR}/lib/cmake/TBB")
endif()

macro(copy_tbb_libs TBB_LIBS_COPY_DIR TBB_LIBS_COPY_DEPEND TARGET_DEPEND_ON_TBB_LIBS)
  if(THREADING STREQUAL "TBB")
    if(TARGET tbb_libs_copy)
      message(FATAL_ERROR "Target tbb_libs_copy already exists. Incorrect usage of copy_tbb_libs macro.")
    endif()
    add_custom_target(tbb_libs_copy
      COMMAND cp -a ${ONETBB_INSTALL_DIR}/lib/*.so* ${TBB_LIBS_COPY_DIR}/
      DEPENDS ${TBB_LIBS_COPY_DEPEND} ${ONETBB_BUILD})
    add_dependencies(${TARGET_DEPEND_ON_TBB_LIBS} tbb_libs_copy)
  endif()
endmacro()
