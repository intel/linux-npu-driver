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
set(OPENVINO_REVISION 0ebff040fd22daa37612a82fdf930ffce4ebb099)
set(VPUX_PLUGIN_REVISION 4f89b7c000d98bb8f4bf5bb058967bbd02834caa)
set(VPUX_PLUGIN_RELEASE npu_ud_2024_44_rc1)
set(OPENCV_REVISION 78195bc3dfe20b96e721ae8b32d0aa3491755e78)

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
