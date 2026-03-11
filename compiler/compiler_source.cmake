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
set(OPENVINO_REVISION 77501947b70f6c7996f07c068f2067e868e356ed)
set(OPENCV_REVISION dee619b628e86d187bbe7a55b5fe6c7bfbb8b64d)
set(GENAI_REVISION 67e82bbdcadd740b7d84de7b6586fb1a92836789)
set(ONNXRUNTIME_TAG microsoft:50d4c84724d862e94140e8b3388633db376800a8)
set(ONNXRUNTIME_REVISION 50d4c84724d862e94140e8b3388633db376800a8)

set(NPU_COMPILER_TAG npu_ud_2026_08_rc2)
set(NPU_COMPILER_REVISION e0af53718947347b12ffcb7c8b7499ac7f2e93bf)
# Compiler might use different OpenVINO revision
set(NPU_COMPILER_OPENVINO_REVISION 4922c4955f9d5c457cf9d4ebbbc8bf6502167ada)

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
