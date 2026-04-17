# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT

if(TARGET npu_compiler_source)
  return()
endif()

if(DEFINED ENV{TARGET_DISTRO})
  set(TARGET_DISTRO $ENV{TARGET_DISTRO})
else()
  set(TARGET_DISTRO ${CMAKE_SYSTEM_NAME})
endif()

include(ExternalProject)

set(OPENVINO_REPOSITORY https://github.com/openvinotoolkit/openvino.git)
set(OPENVINO_REVISION f3a30a671d32c8e99cc496f0cc9d0089711fbb82)
set(OPENCV_REVISION c1c893ff73581567b6d1a847e8dafc9c052d907c)
set(GENAI_REVISION adc69c578f02b9b07aaf0d15ff35747ae3962a52)
set(ONNXRUNTIME_TAG microsoft:058787ceead760166e3c50a0a4cba8a833a6f53f)
set(ONNXRUNTIME_REVISION 058787ceead760166e3c50a0a4cba8a833a6f53f)

set(NPU_COMPILER_TAG npu_ud_2026_12_1_rc1)
set(NPU_COMPILER_REVISION da4d79af8cb4bef6a71394bf0e22d998e5fe5b3a)
# Compiler might use different OpenVINO revision
set(NPU_COMPILER_OPENVINO_REVISION b7f9dbfa7944a1c576b87a4061c298636ae9f2a7)

set(OPENVINO_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/src/openvino")
file(MAKE_DIRECTORY ${OPENVINO_SOURCE_DIR})

list(APPEND DISABLE_GIT_LFS "filter.lfs.smudge=git-lfs smudge --skip -- %f")
list(APPEND DISABLE_GIT_LFS "filter.lfs.process=git-lfs filter-process --skip")

ExternalProject_Add(
  openvino_source
  GIT_REPOSITORY ${OPENVINO_REPOSITORY}
  GIT_TAG ${OPENVINO_REVISION}
  GIT_CONFIG ${DISABLE_GIT_LFS}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  # Only change the content of source dir in case revision or patch command change
  UPDATE_DISCONNECTED TRUE)

set(NPU_COMPILER_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/src/npu_compiler")
file(MAKE_DIRECTORY ${NPU_COMPILER_SOURCE_DIR})

if(NOT NPU_COMPILER_OPENVINO_REVISION STREQUAL OPENVINO_REVISION)
  set(NPU_COMPILER_OPENVINO_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/npu_compiler_openvino)
  file(MAKE_DIRECTORY ${NPU_COMPILER_OPENVINO_SOURCE_DIR})

  set(NPU_COMPILER_BUILD_DEPENDS npu_compiler_openvino_source)

  # Copy the local openvino_source and checkout for the compiler the required OpenVINO revision
  ExternalProject_Add(
    npu_compiler_openvino_source
    DEPENDS openvino_source
    GIT_REPOSITORY ${OPENVINO_SOURCE_DIR}
    GIT_TAG ${NPU_COMPILER_OPENVINO_REVISION}
    GIT_CONFIG ${DISABLE_GIT_LFS}
    SOURCE_DIR ${NPU_COMPILER_OPENVINO_SOURCE_DIR}
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    # Only change the content of source dir in case revision or patch command change
    UPDATE_DISCONNECTED TRUE)
else()
  set(NPU_COMPILER_OPENVINO_SOURCE_DIR ${OPENVINO_SOURCE_DIR})
  set(NPU_COMPILER_BUILD_DEPENDS openvino_source)
endif()

ExternalProject_Add(
  npu_compiler_source
  GIT_REPOSITORY
    https://github.com/openvinotoolkit/npu_compiler.git
  GIT_TAG ${NPU_COMPILER_REVISION}
  GIT_CONFIG ${DISABLE_GIT_LFS}
  SOURCE_DIR ${NPU_COMPILER_SOURCE_DIR}
  PATCH_COMMAND
    git -C ${NPU_COMPILER_SOURCE_DIR} lfs install &&
    git -C ${NPU_COMPILER_SOURCE_DIR} lfs pull &&
    git -C ${NPU_COMPILER_SOURCE_DIR}/thirdparty/vpucostmodel lfs install &&
    git -C ${NPU_COMPILER_SOURCE_DIR}/thirdparty/vpucostmodel lfs pull
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  # Only change the content of source dir in case revision or patch command change
  UPDATE_DISCONNECTED TRUE)
