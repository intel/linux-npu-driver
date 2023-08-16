# Copyright 2022-2023 Intel Corporation.
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
set(OPENVINO_REVISION 691630b68ca76e242089f0f1229a2a3ca8e88182)
set(VPUX_PLUGIN_REVISION 914dd101157020a97b860718d5baf53a4d155d56)
set(VPUX_PLUGIN_RELEASE 23ww32-vpu37xx-pv)

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
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Add(
  vpux_plugin_source
  GIT_REPOSITORY
    https://github.com/openvinotoolkit/vpux_plugin.git
  GIT_TAG ${VPUX_PLUGIN_REVISION}
  PREFIX ${VPUX_PLUGIN_PREFIX_DIR}
  SOURCE_DIR ${VPUX_PLUGIN_SOURCE_DIR}
  PATCH_COMMAND
    git -C ${VPUX_PLUGIN_SOURCE_DIR}/thirdparty/vpucostmodel lfs pull
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")
