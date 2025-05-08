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

include(compiler_source.cmake)

include(ProcessorCount)
ProcessorCount(PARALLEL_PROCESSES)

set(NPU_COMPILER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/build_npu_compiler")
file(MAKE_DIRECTORY ${NPU_COMPILER_BINARY_DIR})

set(THREADING "TBB" CACHE STRING "Build NPU Compiler with specific THREADING option")

set(NPU_COMPILER_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/npu_compiler)
set(NPU_COMPILER_PACKAGE_DIR ${NPU_COMPILER_INSTALL_PREFIX}/cid)

include(ExternalProject)

ExternalProject_Add(
  npu_compiler_build
  DOWNLOAD_COMMAND ""
  DEPENDS npu_compiler_source ${NPU_COMPILER_BUILD_DEPENDS}
  SOURCE_DIR ${NPU_COMPILER_OPENVINO_SOURCE_DIR}
  BINARY_DIR ${NPU_COMPILER_BINARY_DIR}
  CMAKE_ARGS
      -D CMAKE_BUILD_TYPE=Release
      -D CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -D CMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
      -D BUILD_COMPILER_FOR_DRIVER=ON
      -D BUILD_SHARED_LIBS=OFF
      # CLANG_FORMAT and NCC_STYLE is set to OFF to avoid LLVMDemangle doubled target issue
      -D ENABLE_CLANG_FORMAT=OFF
      -D ENABLE_CLANG_TIDY=OFF
      -D ENABLE_NCC_STYLE=OFF
      # Copied from "how_to_build_driver_compiler" document
      -D ENABLE_AUTO=OFF
      -D ENABLE_AUTO_BATCH=OFF
      -D ENABLE_BLOB_DUMP=OFF
      -D ENABLE_FUNCTIONAL_TESTS=OFF
      -D ENABLE_HETERO=OFF
      -D ENABLE_INTEL_CPU=OFF
      -D ENABLE_INTEL_GPU=OFF
      -D ENABLE_JS=OFF
      -D ENABLE_MULTI=OFF
      -D ENABLE_INTEL_NPU_PROTOPIPE=OFF
      -D ENABLE_OV_IR_FRONTEND=ON
      -D ENABLE_OV_JAX_FRONTEND=OFF
      -D ENABLE_OV_ONNX_FRONTEND=OFF
      -D ENABLE_OV_PADDLE_FRONTEND=OFF
      -D ENABLE_OV_PYTORCH_FRONTEND=OFF
      -D ENABLE_OV_TF_FRONTEND=OFF
      -D ENABLE_OV_TF_LITE_FRONTEND=OFF
      -D ENABLE_PROXY=OFF
      -D ENABLE_SAMPLES=OFF
      -D ENABLE_TBBBIND_2_5=OFF
      -D ENABLE_TEMPLATE=OFF
      -D ENABLE_TESTS=OFF
      # WA in case libgflags is installed in system
      -D gflags_DIR=${CMAKE_CURRENT_SOURCE_DIR}/openvino_modules
      -D OPENVINO_EXTRA_MODULES=${NPU_COMPILER_SOURCE_DIR}
      -D OUTPUT_ROOT=${NPU_COMPILER_BINARY_DIR}
      -D THREADING=${THREADING}
  BUILD_COMMAND
    ${CMAKE_COMMAND}
      --build ${NPU_COMPILER_BINARY_DIR}
      --config Release
      --target compilerTest profilingTest vpuxCompilerL0Test loaderTest
      --parallel ${PARALLEL_PROCESSES}
  INSTALL_COMMAND
    ${CMAKE_COMMAND}
      --install ${NPU_COMPILER_BINARY_DIR}
      --prefix ${NPU_COMPILER_INSTALL_PREFIX}
      --component CiD
  BYPRODUCTS
    ${NPU_COMPILER_PACKAGE_DIR}/lib/libnpu_driver_compiler.so
    ${NPU_COMPILER_PACKAGE_DIR}/vpux_elf/lib/Release/libnpu_elf.a
)

add_dependencies(npu_compiler npu_compiler_build)

# Extra command to prepare a standalone package with NPU compiler
string(REPLACE "/" "_" NPU_COMPILER_TAG_PACKAGE ${NPU_COMPILER_TAG})
set(NPU_COMPILER_PACKAGE_NAME "npu-drv-compiler-${TARGET_DISTRO}-${NPU_COMPILER_TAG_PACKAGE}-${BUILD_NUMBER}")

add_custom_target(npu_compiler_package
  COMMAND
    tar -C ${NPU_COMPILER_INSTALL_PREFIX} -czf ${CMAKE_BINARY_DIR}/${NPU_COMPILER_PACKAGE_NAME}.tar.gz --transform='s,/cid,/npu_compiler,' ./cid/
  DEPENDS npu_compiler_build
  BYPRODUCTS ${CMAKE_BINARY_DIR}/${NPU_COMPILER_PACKAGE_NAME}.tar.gz)

install(
  FILES ${CMAKE_BINARY_DIR}/${NPU_COMPILER_PACKAGE_NAME}.tar.gz
  DESTINATION .
  COMPONENT npu_compiler_package
  EXCLUDE_FROM_ALL)
