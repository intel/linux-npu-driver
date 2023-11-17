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

if(NOT ENABLE_OPENVINO_PACKAGE)
  return()
endif()

include(compiler_source.cmake)

if(DEFINED ENV{TARGET_DISTRO})
  set(TARGET_DISTRO $ENV{TARGET_DISTRO})
else()
  set(TARGET_DISTRO ${CMAKE_SYSTEM_NAME})
endif()

set(OPENCV_PREFIX_DIR "${CMAKE_BINARY_DIR}/third_party/opencv")
set(OPENCV_SOURCE_DIR "${OPENCV_PREFIX_DIR}/src/opencv")
file(MAKE_DIRECTORY ${OPENCV_SOURCE_DIR})

set(OPENCV_BINARY_DIR "${OPENCV_PREFIX_DIR}/build")
file(MAKE_DIRECTORY ${OPENCV_BINARY_DIR})

set(OPENVINO_PACKAGE_NAME "openvino-vpu-drv-${TARGET_DISTRO}-${VPUX_PLUGIN_RELEASE}-${VPU_VERSION}")

set(OPENVINO_BINARY_DIR "${OPENVINO_PREFIX_DIR}/build")
file(MAKE_DIRECTORY ${OPENVINO_BINARY_DIR})

set(OPENVINO_PACKAGE_DIR "${CMAKE_BINARY_DIR}/third_party/openvino_package")
file(MAKE_DIRECTORY ${OPENVINO_PACKAGE_DIR})

set(SAMPLES_APPS_PACKAGE_DIR "${OPENVINO_PACKAGE_DIR}/tools/deployment_tools/inference_engine/bin")
file(MAKE_DIRECTORY ${SAMPLES_APPS_PACKAGE_DIR})

set(OPENVINO_BINARY_RELEASE_DIR "${OPENVINO_SOURCE_DIR}/bin/intel64/Release")

### OpenVINO flags
list(APPEND OPENVINO_CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Release)
list(APPEND OPENVINO_CMAKE_FLAGS -DCMAKE_INSTALL_PREFIX=${OPENVINO_PACKAGE_DIR})
# To set BUILD_SHARED_LIBS to OFF the cmake 3.18 is required
list(APPEND OPENVINO_CMAKE_FLAGS -DBUILD_SHARED_LIBS=ON)
# OpenVINO Plugins
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_INTEL_CPU=ON)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_INTEL_GPU=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_INTEL_GNA=ON)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_HETERO=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_MULTI=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_AUTO=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_TEMPLATE=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_AUTO_BATCH=OFF)
# OpenVINO Frontends
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_OV_IR_FRONTEND=ON)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_OV_TF_FRONTEND=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_OV_TF_LITE_FRONTEND=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_OV_ONNX_FRONTEND=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_OV_PADDLE_FRONTEND=OFF)
# OpenVINO Other
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_PLUGINS_XML=ON)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_IR_V7_READER=ON)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_SAMPLES=ON)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_PYTHON=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_WHEEL=OFF)
list(APPEND OPENVINO_CMAKE_FLAGS -DTHREADING=TBB)
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_TBBBIND_2_5=OFF)
# OpenCV is required by single-image-test in vpux-plugin
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_OPENCV=ON)

### VPUX plugin flags
list(APPEND OPENVINO_CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Release)
list(APPEND OPENVINO_CMAKE_FLAGS -DIE_EXTRA_MODULES=${VPUX_PLUGIN_SOURCE_DIR})
# MLIR can be used in compile_tool
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_MLIR_COMPILER=ON)
# Enable using CiD from VPUX plugin
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_DRIVER_COMPILER_ADAPTER=ON)
# Do not build CiD, the CiD is comes with driver
list(APPEND OPENVINO_CMAKE_FLAGS -DBUILD_COMPILER_FOR_DRIVER=OFF)
# To build CiD, ENABLE_TESTS has to be set to ON
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_TESTS=OFF)
# Below flags add logs that can be enabled using "export IE_VPUX_LOG_LEVEL=LOG_INFO"
list(APPEND OPENVINO_CMAKE_FLAGS -DENABLE_DEVELOPER_BUILD=OFF)

# TODO: Temporary WA for building LLVM from plugin source
list(APPEND OPENVINO_CMAKE_FLAGS -DLLVM_ENABLE_ZLIB=OFF)
# TODO: Disable AVX512F instruction for MTL to exclude unsupported instructions
list(APPEND OPENVINO_CMAKE_FLAGS -DCMAKE_CXX_FLAGS_INIT="-mno-avx512f")

list(APPEND OPENVINO_CMAKE_FLAGS -DOpenCV_DIR=${OPENCV_BINARY_DIR})

ExternalProject_Add(
  opencv_build
  GIT_REPOSITORY https://github.com/opencv/opencv.git
  GIT_TAG 4.8.0
  PREFIX ${OPENCV_PREFIX_DIR}
  SOURCE_DIR ${OPENCV_SOURCE_DIR}
  BINARY_DIR ${OPENCV_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}/opencv
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${OPENVINO_PACKAGE_DIR}/opencv
             -DOPENCV_GENERATE_SETUPVARS=ON)

ExternalProject_Add(
  openvino_vpux_plugin_build
  DOWNLOAD_COMMAND ""
  DEPENDS vpux_plugin_source openvino_source opencv_build
  PREFIX ${OPENVINO_PREFIX_DIR}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}
  BINARY_DIR ${OPENVINO_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}
  CMAKE_ARGS ${OPENVINO_CMAKE_FLAGS})

add_custom_target(
  openvino_package ALL
  COMMAND
    cp -d ${OPENCV_BINARY_DIR}/setup_vars.sh ${OPENVINO_PACKAGE_DIR}/opencv/setupvars.sh &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/benchmark_app ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/classification_sample_async ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/hello_classification ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/hello_query_device ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/single-image-test ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    git -C ${OPENVINO_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/openvino_sha &&
    git -C ${VPUX_PLUGIN_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/vpux_plugin_sha &&
    echo ${OPENVINO_PACKAGE_NAME} > ${OPENVINO_PACKAGE_DIR}/build_version &&
    echo `git -C ${OPENVINO_SOURCE_DIR} rev-parse HEAD` `git -C ${OPENVINO_SOURCE_DIR} config --local --get remote.origin.url` > ${OPENVINO_PACKAGE_DIR}/manifest.txt &&
    echo `git -C ${VPUX_PLUGIN_SOURCE_DIR} rev-parse HEAD` `git -C ${VPUX_PLUGIN_SOURCE_DIR} config --local --get remote.origin.url` >> ${OPENVINO_PACKAGE_DIR}/manifest.txt &&
    tar -C ${OPENVINO_PACKAGE_DIR} -czf ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz .
  DEPENDS openvino_vpux_plugin_build
  BYPRODUCTS ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz)

install(
  FILES ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz
  DESTINATION .
  COMPONENT openvino_package)
