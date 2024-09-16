# Copyright 2022-2024 Intel Corporation.
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

if(TARGET npu_plugin_source)
  return()
endif()

include(ExternalProject)

# OpenVINO + NPU Plugin package options
set(OPENVINO_REVISION 7edb05f29487cbf5cc6a7d7ae0a8e228aac763b0)
set(VPUX_PLUGIN_REVISION 1a83394af6430a8a4a29dfc6faf50ed26594e387)
set(VPUX_PLUGIN_RELEASE npu_ud_2024_36_rc1)
set(OPENCV_REVISION 5dc1b39e4c9dfb3339e0b910f7d824a02474ceed)

# Directories
set(OPENVINO_PREFIX_DIR "${CMAKE_BINARY_DIR}/third_party/openvino")
set(OPENVINO_SOURCE_DIR "${OPENVINO_PREFIX_DIR}/src/openvino")
file(MAKE_DIRECTORY ${OPENVINO_SOURCE_DIR})

set(NPU_PLUGIN_PREFIX_DIR "${CMAKE_BINARY_DIR}/third_party/npu_plugin")
set(NPU_PLUGIN_SOURCE_DIR "${NPU_PLUGIN_PREFIX_DIR}/src/npu_plugin")
file(MAKE_DIRECTORY ${NPU_PLUGIN_SOURCE_DIR})

ExternalProject_Add(
  openvino_source
  GIT_REPOSITORY https://github.com/openvinotoolkit/openvino.git
  GIT_TAG ${OPENVINO_REVISION}
  PREFIX ${OPENVINO_PREFIX_DIR}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}
  UPDATE_DISCONNECTED TRUE
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Add(
  npu_plugin_source
  GIT_REPOSITORY
    https://github.com/openvinotoolkit/npu_plugin.git
  GIT_TAG ${VPUX_PLUGIN_REVISION}
  PREFIX ${NPU_PLUGIN_PREFIX_DIR}
  SOURCE_DIR ${NPU_PLUGIN_SOURCE_DIR}
  UPDATE_DISCONNECTED TRUE
  PATCH_COMMAND
    git -C ${NPU_PLUGIN_SOURCE_DIR} lfs install &&
    git -C ${NPU_PLUGIN_SOURCE_DIR} lfs pull &&
    git -C ${NPU_PLUGIN_SOURCE_DIR}/thirdparty/vpucostmodel lfs install &&
    git -C ${NPU_PLUGIN_SOURCE_DIR}/thirdparty/vpucostmodel lfs pull
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")
