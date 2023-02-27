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

if(NOT ENABLE_VPUX_COMPILER)
  return()
endif()

include(ExternalProject)
include(ProcessorCount)

set(OPENVINO_DIR "${CMAKE_BINARY_DIR}/third_party/openvino")
set(OPENVINO_BINARY_DIR "${OPENVINO_DIR}/build")
set(VPUX_PLUGIN_DIR "${CMAKE_BINARY_DIR}/third_party/vpux_plugin")
set(VPUX_COMPILER_INCLUDE_DIR "${VPUX_PLUGIN_DIR}/src/VPUXCompilerL0/include")
set(VPUX_COMPILER_BINARY_DIR "${OPENVINO_DIR}/bin/intel64/Release")

ProcessorCount(PARALLEL_PROCESSES)

ExternalProject_Add(
  openvino
  GIT_REPOSITORY https://github.com/openvinotoolkit/openvino.git
  GIT_TAG 0a5ca5375265f0f12cdaee68574030408dd1c352
  SOURCE_DIR ${OPENVINO_DIR}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Add(
  vpux_compiler
  GIT_REPOSITORY
    https://github.com/openvinotoolkit/vpux_plugin.git
  GIT_TAG vpu_chrome_alpha_rc1
  GIT_SHALLOW ON
  DEPENDS openvino
  EXCLUDE_FROM_ALL ON
  SOURCE_DIR ${VPUX_PLUGIN_DIR}
  PATCH_COMMAND
    cd ${VPUX_PLUGIN_DIR} &&
    git lfs pull &&
    git submodule foreach git lfs pull
  CONFIGURE_COMMAND
    mkdir -p ${OPENVINO_BINARY_DIR} &&
    ${CMAKE_COMMAND}
      -D ENABLE_TESTS=ON
      -D ENABLE_CLDNN=OFF
      -D ENABLE_BLOB_DUMP=OFF
      -D ENABLE_HETERO=OFF
      -D ENABLE_MULTI=OFF
      -D ENABLE_TEMPLATE=OFF
      -D ENABLE_IR_V7_READER=OFF
      -D ENABLE_OV_ONNX_FRONTEND=OFF
      -D ENABLE_OV_PADDLE_FRONTEND=OFF
      -D ENABLE_OV_TF_FRONTEND=OFF
      -D ENABLE_GAPI_PREPROCESSING=OFF
      -D ENABLE_INTEL_CPU=OFF
      -D ENABLE_INTEL_GNA=OFF
      -D ENABLE_INTEL_GPU=OFF
      -D ENABLE_OV_IR_FRONTEND=ON
      -D ENABLE_OV_CORE_UNIT_TESTS=OFF
      -D IE_EXTRA_MODULES=${VPUX_PLUGIN_DIR}
      -D BUILD_SHARED_LIBS=OFF
      -D BUILD_COMPILER_FOR_DRIVER=ON
      -D ENABLE_CLANG_FORMAT=OFF
      -D CMAKE_BUILD_TYPE=Release
      -S ${OPENVINO_DIR}
      -B ${OPENVINO_BINARY_DIR}
  BUILD_COMMAND
    ${CMAKE_COMMAND}
      --build ${OPENVINO_BINARY_DIR}
      --target VPUXCompilerL0
      --parallel ${PARALLEL_PROCESSES}
  INSTALL_COMMAND
    cp ${VPUX_COMPILER_BINARY_DIR}/libVPUXCompilerL0.so ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/)

install(FILES ${CMAKE_BINARY_DIR}/lib/libVPUXCompilerL0.so TYPE LIB)
add_compile_definitions(ENABLE_VPUX_COMPILER)
