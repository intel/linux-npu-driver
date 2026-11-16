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
set(LEVEL_ZERO_VERSION "1.34.0")

if(NOT ENABLE_LEVEL_ZERO_FROM_SUBMODULE)
  find_package(LevelZero ${LEVEL_ZERO_VERSION})
endif()
if(NOT ENABLE_LEVEL_ZERO_FROM_SUBMODULE
   AND NOT LevelZero_FOUND
   AND LINUX_SYSTEM_NAME STREQUAL "ubuntu"
   AND (LINUX_SYSTEM_VERSION_ID STREQUAL "24.04"
        OR LINUX_SYSTEM_VERSION_ID STREQUAL "26.04"))

  # libze package source:
  # https://launchpad.net/~kobuk-team/+archive/ubuntu/intel-graphics/+files/libze1*
  # https://launchpad.net/~kobuk-team/+archive/ubuntu/intel-graphics/+files/libze-dev*
  set(PKG_NAMES libze1;libze-dev)
  set(PPA_URL https://snapshot.ppa.launchpadcontent.net/kobuk-team/intel-graphics/ubuntu/20261110T100000Z/pool/main/l/level-zero-loader)

  if(LINUX_SYSTEM_VERSION_ID STREQUAL "24.04")
    set(UBUNTU_PPA_SUFFIX "24.04")
    set(PKG_MD5S c2b5f30fe2f9527d6fcfbdf4bf0cdc62;e50b98278fcb79fd21b4d8264b90aa7e)
  elseif(LINUX_SYSTEM_VERSION_ID STREQUAL "26.04")
    set(UBUNTU_PPA_SUFFIX "26.04")
    set(PKG_MD5S 2dd901447c21629294da5f66d91fcdca;dbfdf80f0042813e7b3d29cfe32160af)
  endif()

  foreach(PKG_NAME PKG_MD5 IN ZIP_LISTS PKG_NAMES PKG_MD5S)
    set(PKG_URL
          ${PPA_URL}/${PKG_NAME}_${LEVEL_ZERO_VERSION}-1~${UBUNTU_PPA_SUFFIX}~ppa1_amd64.deb
    )

    message(STATUS "Downloading LevelZero package: ${PKG_NAME} from ${PKG_URL}")
    FetchContent_Declare(
      ${PKG_NAME}
      URL ${PKG_URL}
      URL_HASH MD5=${PKG_MD5}
      SOURCE_DIR ${CMAKE_BINARY_DIR}
      DOWNLOAD_NO_EXTRACT TRUE)
    FetchContent_MakeAvailable(${PKG_NAME})
    execute_process(
      COMMAND
        dpkg -x
        ${${PKG_NAME}_SOURCE_DIR}/${PKG_NAME}_${LEVEL_ZERO_VERSION}-1~${UBUNTU_PPA_SUFFIX}~ppa1_amd64.deb
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
