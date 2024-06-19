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

if(NOT ENABLE_OPENVINO_PACKAGE)
  return()
endif()

include(compiler_source.cmake)

if(DEFINED ENV{TARGET_DISTRO})
  set(TARGET_DISTRO $ENV{TARGET_DISTRO})
else()
  set(TARGET_DISTRO ${CMAKE_SYSTEM_NAME})
endif()

set(OPENVINO_PACKAGE_NAME "openvino-vpu-drv-${TARGET_DISTRO}-${VPUX_PLUGIN_RELEASE}-${BUILD_NUMBER}")
set(OPENVINO_PACKAGE_DIR "${CMAKE_BINARY_DIR}/third_party/openvino_package")
file(MAKE_DIRECTORY ${OPENVINO_PACKAGE_DIR})

list(APPEND COMMON_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
list(APPEND COMMON_CMAKE_ARGS -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM})

set(THREADING "TBB" CACHE STRING "Build OpenVINO with specific THREADING option")

### OpenVINO ###
set(OPENVINO_BINARY_DIR "${OPENVINO_PREFIX_DIR}/build")
file(MAKE_DIRECTORY ${OPENVINO_BINARY_DIR})

ExternalProject_Add(
  openvino_build
  DOWNLOAD_COMMAND ""
  DEPENDS vpux_plugin_source openvino_source
  PREFIX ${OPENVINO_PREFIX_DIR}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}
  BINARY_DIR ${OPENVINO_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_PREFIX=${OPENVINO_PACKAGE_DIR}
    -DENABLE_PYTHON=OFF
    -DENABLE_NCC_STYLE=OFF
    -DENABLE_CLANG_FORMAT=OFF
    -DTHREADING=${THREADING})

### OpenCV ###
set(OPENCV_PREFIX_DIR "${CMAKE_BINARY_DIR}/third_party/opencv")
set(OPENCV_SOURCE_DIR "${OPENCV_PREFIX_DIR}/src/opencv")
file(MAKE_DIRECTORY ${OPENCV_SOURCE_DIR})

set(OPENCV_BINARY_DIR "${OPENCV_PREFIX_DIR}/build")
file(MAKE_DIRECTORY ${OPENCV_BINARY_DIR})

ExternalProject_Add(
  opencv_build
  GIT_REPOSITORY https://github.com/opencv/opencv.git
  GIT_TAG 8e43c8f200b1b785df7f265dfa79ee97278977f0
  DEPENDS openvino_build
  UPDATE_DISCONNECTED TRUE
  PATCH_COMMAND ""
  PREFIX ${OPENCV_PREFIX_DIR}
  SOURCE_DIR ${OPENCV_SOURCE_DIR}
  BINARY_DIR ${OPENCV_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}/opencv
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${OPENVINO_PACKAGE_DIR}/opencv
    -DCMAKE_PREFIX_PATH=${OPENVINO_BINARY_DIR}
    -DOPENCV_GENERATE_SETUPVARS=ON
    -DBUILD_opencv_dnn=OFF
    -DWITH_EIGEN=OFF
    -DWITH_JASPER=OFF
    -DWITH_OPENCL=OFF
    -DWITH_OPENEXR=OFF
    -DWITH_OPENJPEG=OFF
    -DWITH_OPENVINO=ON
    -DWITH_TIFF=OFF
    -DWITH_WEBP=OFF)

### VPUX plugin ###
set(VPUX_PLUGIN_BINARY_DIR ${VPUX_PLUGIN_PREFIX_DIR}/build)
file(MAKE_DIRECTORY ${VPUX_PLUGIN_BINARY_DIR})

ExternalProject_Add(
  vpux_plugin_build
  DOWNLOAD_COMMAND ""
  DEPENDS openvino_build opencv_build
  PREFIX ${VPUX_PLUGIN_PREFIX_DIR}
  SOURCE_DIR ${VPUX_PLUGIN_SOURCE_DIR}
  BINARY_DIR ${VPUX_PLUGIN_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_PREFIX=${OPENVINO_PACKAGE_DIR}
    -DOpenCV_DIR=${OPENCV_BINARY_DIR}
    -DInferenceEngineDeveloperPackage_DIR=${OPENVINO_BINARY_DIR}
    -DTHREADING=${THREADING})

### Sample applications from OpenVINO (benchmark_app ...) ###
set(SAMPLES_APPS_BUILD_DIR ${OPENVINO_PREFIX_DIR}/build-samples)
file(MAKE_DIRECTORY ${SAMPLES_APPS_BUILD_DIR})

set(SAMPLES_APPS_PACKAGE_DIR "${OPENVINO_PACKAGE_DIR}/tools/deployment_tools/inference_engine/bin")
file(MAKE_DIRECTORY ${SAMPLES_APPS_PACKAGE_DIR})

ExternalProject_Add(
  sample_apps_build
  DOWNLOAD_COMMAND ""
  DEPENDS openvino_build opencv_build
  PREFIX ${OPENVINO_PREFIX_DIR}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}/samples/cpp
  BINARY_DIR ${SAMPLES_APPS_BUILD_DIR}
  INSTALL_DIR ${SAMPLES_APPS_BUILD_DIR}
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${SAMPLES_APPS_PACKAGE_DIR}
    -DCMAKE_PREFIX_PATH=${OPENVINO_BINARY_DIR}
    -DOpenCV_DIR=${OPENCV_BINARY_DIR}
    -DSAMPLES_ENABLE_OPENCL=OFF)

### OV+VPUX plugin package ###
set(COMPILE_TOOL_PACKAGE_DIR "${OPENVINO_PACKAGE_DIR}/tools/compile_tool")
file(MAKE_DIRECTORY ${COMPILE_TOOL_PACKAGE_DIR})

set(OPENVINO_BINARY_RELEASE_DIR "${OPENVINO_SOURCE_DIR}/bin/intel64/Release")

add_custom_target(
  openvino_package ALL
  COMMAND
    cp -d ${OPENCV_BINARY_DIR}/setup_vars.sh ${OPENVINO_PACKAGE_DIR}/opencv/setupvars.sh &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/benchmark_app ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/classification_sample_async ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/hello_classification ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/hello_query_device ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/protopipe ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/single-image-test ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${OPENVINO_BINARY_RELEASE_DIR}/compile_tool ${COMPILE_TOOL_PACKAGE_DIR}/ &&
    git -C ${OPENCV_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/opencv_sha &&
    git -C ${OPENVINO_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/openvino_sha &&
    git -C ${VPUX_PLUGIN_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/vpux_plugin_sha &&
    echo ${OPENVINO_PACKAGE_NAME} > ${OPENVINO_PACKAGE_DIR}/build_version &&
    echo `git -C ${OPENVINO_SOURCE_DIR} rev-parse HEAD` `git -C ${OPENVINO_SOURCE_DIR} config --local --get remote.origin.url` > ${OPENVINO_PACKAGE_DIR}/manifest.txt &&
    echo `git -C ${VPUX_PLUGIN_SOURCE_DIR} rev-parse HEAD` `git -C ${VPUX_PLUGIN_SOURCE_DIR} config --local --get remote.origin.url` >> ${OPENVINO_PACKAGE_DIR}/manifest.txt &&
    tar -C ${OPENVINO_PACKAGE_DIR} -czf ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz .
  DEPENDS openvino_build opencv_build vpux_plugin_build sample_apps_build
  BYPRODUCTS ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz)

install(
  FILES ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz
  DESTINATION .
  COMPONENT openvino_standalone_package
  EXCLUDE_FROM_ALL)

install(PROGRAMS
            ${SAMPLES_APPS_BUILD_DIR}/intel64/benchmark_app
            ${SAMPLES_APPS_BUILD_DIR}/intel64/classification_sample_async
            ${SAMPLES_APPS_BUILD_DIR}/intel64/hello_classification
            ${SAMPLES_APPS_BUILD_DIR}/intel64/hello_query_device
            ${OPENVINO_BINARY_RELEASE_DIR}/protopipe
            ${OPENVINO_BINARY_RELEASE_DIR}/single-image-test
            ${OPENVINO_BINARY_RELEASE_DIR}/compile_tool
        COMPONENT openvino-npu
        TYPE BIN)

install(DIRECTORY ${OPENVINO_BINARY_RELEASE_DIR}/
        COMPONENT openvino-npu
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILES_MATCHING
        PATTERN "libnpu_driver_compiler_adapter.so"
        PATTERN "libnpu_level_zero_backend.so"
        PATTERN "libnpu_mlir_compiler.so"
        PATTERN "libopenvino.so*"
        PATTERN "libopenvino_intel_cpu_plugin.so"
        PATTERN "libopenvino_intel_gpu_plugin.so"
        PATTERN "libopenvino_intel_npu_plugin.so"
        PATTERN "libopenvino_*_frontend.so*"
        PATTERN "plugins.xml")

install(DIRECTORY ${OPENCV_BINARY_DIR}/lib/
        COMPONENT openvino-npu
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILES_MATCHING
        PATTERN "libopencv_calib3d.so*"
        PATTERN "libopencv_core.so*"
        PATTERN "libopencv_dnn.so*"
        PATTERN "libopencv_features2d.so*"
        PATTERN "libopencv_flann.so*"
        PATTERN "libopencv_gapi.so*"
        PATTERN "libopencv_imgcodecs.so*"
        PATTERN "libopencv_imgproc.so*"
        PATTERN "libopencv_video.so*"
        PATTERN "python3" EXCLUDE)
