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
set(OPENVINO_REVISION e213a14725769a6a2bf86fc86650aa78a9c05d3e)
set(OPENCV_REVISION 8ec62ad3460fd4f371000157cf98cccaaa933303)
set(GENAI_REVISION ab01d1771e4adaa0e1e08e9d630def170572506c)
set(ONNXRUNTIME_TAG f2c39fe)
set(ONNXRUNTIME_REVISION f2c39fe)

set(NPU_COMPILER_TAG npu_ud_2026_44_rc1)
set(NPU_COMPILER_REVISION 3009fe6a0e38031c9e6e459dc96507dc1c6593c5)
# Compiler might use different OpenVINO revision
set(NPU_COMPILER_OPENVINO_REVISION 88f8c6bf7c46bb2e3b3fc6c6c357a030a43c780c)

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
