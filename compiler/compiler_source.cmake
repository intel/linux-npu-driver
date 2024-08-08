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

if(TARGET vpux_plugin_source)
  return()
endif()

include(ExternalProject)

# OpenVINO + VPUX Plugin package options
set(OPENVINO_REVISION 7edb05f29487cbf5cc6a7d7ae0a8e228aac763b0)
set(VPUX_PLUGIN_REVISION 593bbd4f19605993bdb4948b3b2db6061f63d257)
set(VPUX_PLUGIN_RELEASE npu_ud_2024_32_rc1)
set(OPENCV_REVISION 5dc1b39e4c9dfb3339e0b910f7d824a02474ceed)

# Directories
set(OPENVINO_PREFIX_DIR "${CMAKE_BINARY_DIR}/third_party/openvino")
set(OPENVINO_SOURCE_DIR "${OPENVINO_PREFIX_DIR}/src/openvino")
file(MAKE_DIRECTORY ${OPENVINO_SOURCE_DIR})

set(VPUX_PLUGIN_PREFIX_DIR "${CMAKE_BINARY_DIR}/third_party/vpux_plugin")
set(VPUX_PLUGIN_SOURCE_DIR "${VPUX_PLUGIN_PREFIX_DIR}/src/vpux_plugin")
file(MAKE_DIRECTORY ${VPUX_PLUGIN_SOURCE_DIR})

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
  vpux_plugin_source
  GIT_REPOSITORY
    https://github.com/openvinotoolkit/npu_plugin.git
  GIT_TAG ${VPUX_PLUGIN_REVISION}
  PREFIX ${VPUX_PLUGIN_PREFIX_DIR}
  SOURCE_DIR ${VPUX_PLUGIN_SOURCE_DIR}
  UPDATE_DISCONNECTED TRUE
  PATCH_COMMAND
    git -C ${VPUX_PLUGIN_SOURCE_DIR}/thirdparty/vpucostmodel lfs install &&
    git -C ${VPUX_PLUGIN_SOURCE_DIR}/thirdparty/vpucostmodel lfs pull
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")
