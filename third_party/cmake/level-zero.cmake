# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

# Make ze_api_headers target to provide Level Zero headers to UMD
add_library(ze_api_headers INTERFACE)
target_include_directories(ze_api_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/level-zero-headers)

if (NOT ENABLE_VALIDATION_BUILD)
    return()
endif()

option(ENABLE_LEVEL_ZERO_FROM_SUBMODULE "Force building Level Zero from submodule" OFF)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(LEVEL_ZERO_VERSION "1.33.1")

if(NOT ENABLE_LEVEL_ZERO_FROM_SUBMODULE)
  find_package(LevelZero ${LEVEL_ZERO_VERSION})
endif()
if(NOT ENABLE_LEVEL_ZERO_FROM_SUBMODULE
   AND NOT LevelZero_FOUND
   AND LINUX_SYSTEM_NAME STREQUAL "ubuntu"
   AND (LINUX_SYSTEM_VERSION_ID STREQUAL "24.04"
        OR LINUX_SYSTEM_VERSION_ID STREQUAL "26.04"))

  # Official libze1/libze-dev .deb packages published upstream:
  # https://github.com/oneapi-src/level-zero/releases/tag/v${LEVEL_ZERO_VERSION}
  # Upstream does not publish a u26.04 build yet, so 26.04 reuses the u24.04 package.
  set(PKG_NAMES libze1;libze-dev)
  set(PKG_URL_BASE https://github.com/oneapi-src/level-zero/releases/download/v${LEVEL_ZERO_VERSION})
  set(LEVEL_ZERO_PKG_SUFFIX "u24.04")
  set(PKG_SHA256S 298d6d3656a13f112497f557be2cb557efb67374bc7d069aa77a9115db909e1d;63eac32aecaaaa5010dbce01e3c309c8edac9bdd80c2fe54c6cbcd77fded714b)

  foreach(PKG_NAME PKG_SHA256 IN ZIP_LISTS PKG_NAMES PKG_SHA256S)
    set(PKG_FILE_NAME ${PKG_NAME}_${LEVEL_ZERO_VERSION}+${LEVEL_ZERO_PKG_SUFFIX}_amd64.deb)
    set(PKG_URL ${PKG_URL_BASE}/${PKG_FILE_NAME})

    message(STATUS "Downloading LevelZero package: ${PKG_NAME} from ${PKG_URL}")
    FetchContent_Declare(
      ${PKG_NAME}
      URL ${PKG_URL}
      URL_HASH SHA256=${PKG_SHA256}
      SOURCE_DIR ${CMAKE_BINARY_DIR}
      DOWNLOAD_NO_EXTRACT TRUE)
    FetchContent_MakeAvailable(${PKG_NAME})
    execute_process(
      COMMAND
        dpkg -x
        ${${PKG_NAME}_SOURCE_DIR}/${PKG_FILE_NAME}
        ${CMAKE_CURRENT_BINARY_DIR}/level-zero/ COMMAND_ERROR_IS_FATAL ANY)
  endforeach()

  set(ENV{PKG_CONFIG_PATH}
      ${CMAKE_CURRENT_BINARY_DIR}/level-zero/usr/lib/x86_64-linux-gnu/pkgconfig:$ENV{PKG_CONFIG_PATH}
  )
  find_package(LevelZero ${LEVEL_ZERO_VERSION})
endif()

if(ENABLE_LEVEL_ZERO_FROM_SUBMODULE)
    set(LevelZero_FOUND FALSE)
endif()

if(NOT LevelZero_FOUND)
  message(STATUS "LevelZero not found. Downloads source from v${LEVEL_ZERO_VERSION} tag")

  FetchContent_Declare(
    level_zero
    GIT_REPOSITORY https://github.com/oneapi-src/level-zero.git
    GIT_TAG "v${LEVEL_ZERO_VERSION}"
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(level_zero)

  target_include_directories(ze_loader SYSTEM INTERFACE ${level_zero_SOURCE_DIR}/include)

  # EXCLUDE_FROM_ALL requires to add components from level-zero manually
  add_dependencies(ze_loader ze_validation_layer ze_tracing_layer)
  install(TARGETS ze_loader ze_validation_layer ze_tracing_layer
          COMPONENT level-zero)
endif()
