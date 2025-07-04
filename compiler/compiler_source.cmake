# Copyright 2022-2025 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this
# software or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express
# or implied warranties, other than those that are expressly stated in
# the License.

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
set(OPENVINO_REVISION 649eec15cfeb1e1dc92164b8886d96637260e487)
set(OPENCV_REVISION 4d6d6fb18fb859f176e5ce2ad3295097a42cd8af)
set(GENAI_REVISION 01f0fe1eded5934871fef866ed217a60fa2c6049)
set(ONNXRUNTIME_REVISION v1.21.1)

set(NPU_COMPILER_TAG  npu_ud_2025_24_rc2)
set(NPU_COMPILER_REVISION b806f941c88c8690a5cab4b50a9760c6daf822db)
# Compiler might use different OpenVINO revision
set(NPU_COMPILER_OPENVINO_REVISION d72b76159445bfa066c13815ff2948fde86dfec2)

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

if(NOT NPU_COMPILER_OPENVINO_REVISION EQUAL OPENVINO_REVISION)
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
    https://github.com/openvinotoolkit/npu_plugin.git
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
