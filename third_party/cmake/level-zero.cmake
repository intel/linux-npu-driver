# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

option(ENABLE_LEVEL_ZERO_FROM_SUBMODULE "Force building Level Zero from submodule" OFF)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(LEVEL_ZERO_VERSION "1.28.2")

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
  set(PPA_URL https://snapshot.ppa.launchpadcontent.net/kobuk-team/intel-graphics/ubuntu/20260606T100000Z/pool/main/l/level-zero-loader)

  if(LINUX_SYSTEM_VERSION_ID STREQUAL "24.04")
    set(UBUNTU_PPA_SUFFIX "24.04")
    set(PKG_MD5S 5c751167369d9c4a44572f4d81a27d0c;fa2be4c30e45b5ebe4b1e1b5c82d2890)
  elseif(LINUX_SYSTEM_VERSION_ID STREQUAL "26.04")
    set(UBUNTU_PPA_SUFFIX "26.04")
     set(PKG_MD5S 236b888efa6c67471a320eec257a9b03;300b9c967385a1aa6d4e05209f0e7df9)
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

if(NOT LevelZero_FOUND)
  message(STATUS "LevelZero not found. Downloads source from v${LEVEL_ZERO_VERSION} tag")
  # TODO: Remove patch when LEVEL_ZERO_VERSION tag includes https://github.com/oneapi-src/level-zero/pull/433
  set(LEVEL_ZERO_PATCHES
    ${CMAKE_CURRENT_SOURCE_DIR}/level-zero-patches/0001-Add-vendor-lib64-to-driver-search-path-for-android.patch)

  FetchContent_Declare(
    level_zero
    GIT_REPOSITORY https://github.com/oneapi-src/level-zero.git
    GIT_TAG "v${LEVEL_ZERO_VERSION}"
    GIT_SHALLOW TRUE
    PATCH_COMMAND
      git -C <SOURCE_DIR> reset --hard HEAD &&
      git -C <SOURCE_DIR> apply ${LEVEL_ZERO_PATCHES}
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(level_zero)

  # Make ze_api_headers target to provide Level Zero headers to ze_loader and other targets
  add_library(ze_api_headers INTERFACE)
  target_include_directories(
    ze_api_headers SYSTEM
    INTERFACE ${level_zero_SOURCE_DIR}/include)

  # Link ze_loader with ze_api_headers to provide Level Zero headers
  target_link_libraries(ze_loader PUBLIC ze_api_headers)

  # EXCLUDE_FROM_ALL requires to add components from level-zero manually
  add_dependencies(ze_loader ze_validation_layer ze_tracing_layer)
  install(TARGETS ze_loader ze_validation_layer ze_tracing_layer
          COMPONENT level-zero)
endif()
