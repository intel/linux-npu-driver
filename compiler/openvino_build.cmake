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

if(NOT ENABLE_OPENVINO_PACKAGE)
  return()
endif()

include(compiler_source.cmake)

string(SUBSTRING ${OPENVINO_REVISION} 0 8 OPENVINO_REVISION_SHORT)
set(OPENVINO_PACKAGE_NAME "openvino-vpu-drv-${TARGET_DISTRO}-${OPENVINO_REVISION_SHORT}-${BUILD_NUMBER}")
set(OPENVINO_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/openvino_package")
file(MAKE_DIRECTORY ${OPENVINO_PACKAGE_DIR})

list(APPEND COMMON_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
list(APPEND COMMON_CMAKE_ARGS -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM})

set(THREADING "TBB" CACHE STRING "Build OpenVINO with specific THREADING option")

### OpenVINO ###
set(OPENVINO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")
file(MAKE_DIRECTORY ${OPENVINO_BINARY_DIR})

set(OPENVINO_CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_PREFIX=${OPENVINO_PACKAGE_DIR}
    -DENABLE_NCC_STYLE=OFF
    -DENABLE_CLANG_FORMAT=OFF
    -DENABLE_CLANG_TIDY=OFF
    -DENABLE_CPPLINT=OFF
    -DENABLE_INTEL_NPU_PROTOPIPE=ON
    -DTHREADING=${THREADING})
set(OPENVINO_BUILD_DEPS openvino_source)
set(BUILD_GENAI OFF)

if (NOT ${LINUX_SYSTEM_NAME} STREQUAL "cros_sdk")
  set(BUILD_GENAI ON)
  set(VENV ${OPENVINO_BINARY_DIR}/venv)
  set(PIP ${OPENVINO_BINARY_DIR}/venv/bin/pip)
  set(WHEELS_DIR ${OPENVINO_BINARY_DIR}/wheels)
  add_custom_target(create_venv ALL
                    COMMAND python3 -m venv ${VENV})
  add_custom_target(requirements_install ALL
                    COMMAND ${PIP} install -U setuptools
                    COMMAND ${PIP} install -r ${OPENVINO_SOURCE_DIR}/src/bindings/python/wheel/requirements-dev.txt
                    DEPENDS create_venv openvino_source)
  list(APPEND OPENVINO_BUILD_DEPS requirements_install)
  list(APPEND OPENVINO_CMAKE_ARGS -DENABLE_WHEEL=ON -DPython3_EXECUTABLE=${VENV}/bin/python)
else()
  list(APPEND OPENVINO_CMAKE_ARGS -DENABLE_PYTHON=OFF)
endif()

ExternalProject_Add(
  openvino_build
  DOWNLOAD_COMMAND ""
  DEPENDS ${OPENVINO_BUILD_DEPS}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}
  BINARY_DIR ${OPENVINO_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    ${OPENVINO_CMAKE_ARGS})

# manually add an interface library for OpenVino so umd tests can link with it
add_library(openvino_library INTERFACE)
add_dependencies(openvino_library openvino_build)
target_include_directories(openvino_library INTERFACE ${OPENVINO_PACKAGE_DIR}/runtime/include)
target_link_libraries(openvino_library INTERFACE ${OPENVINO_PACKAGE_DIR}/runtime/lib/intel64/libopenvino.so)
add_library(openvino::runtime ALIAS openvino_library)

if (${BUILD_GENAI})
  set(GENAI_SOURCE_DIR "${CMAKE_BINARY_DIR}/third_party/openvino.genai")
  ExternalProject_Add(
    genai_build
    DEPENDS openvino_build
    GIT_REPOSITORY https://github.com/openvinotoolkit/openvino.genai
    GIT_TAG ${GENAI_REVISION}
    SOURCE_DIR ${GENAI_SOURCE_DIR}
    UPDATE_DISCONNECTED TRUE
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_COMMAND ${PIP} wheel --verbose --wheel-dir ${WHEELS_DIR} ./thirdparty/openvino_tokenizers/[transformers] --find-links ${WHEELS_DIR} &&
                  ${PIP} wheel --verbose --wheel-dir ${WHEELS_DIR} . --find-links ${WHEELS_DIR}
    BUILD_IN_SOURCE ON)
endif()

if (ENABLE_ONNXRUNTIME_PACKAGE)
    set(ONNXRUNTIME_SOURCE_DIR "${CMAKE_BINARY_DIR}/third_party/onnxruntime")
    ExternalProject_Add(
        onnxruntime_build
        GIT_REPOSITORY https://github.com/microsoft/onnxruntime.git
        GIT_TAG ${ONNXRUNTIME_REVISION}
        DEPENDS openvino_build
        SOURCE_DIR ${ONNXRUNTIME_SOURCE_DIR}
        UPDATE_DISCONNECTED TRUE
        PATCH_COMMAND ""
        CONFIGURE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_COMMAND env OpenVINO_DIR=${OPENVINO_BINARY_DIR} ${ONNXRUNTIME_SOURCE_DIR}/build.sh --use_openvino --skip_tests --parallel --config ${CMAKE_BUILD_TYPE} --cmake_extra_defines "CMAKE_CXX_FLAGS='-Wno-error=redundant-move -Wno-error=free-nonheap-object'"
        BUILD_IN_SOURCE ON)

    set(ONNXRUNTIME_PACKAGE_DIR "${CMAKE_BINARY_DIR}/third_party/onnxruntime-package")
    set(ONNXRUNTIME_PACKAGE_NAME "onnxruntime-${TARGET_DISTRO}-${ONNXRUNTIME_REVISION}-${BUILD_NUMBER}")
    add_custom_target(onnxruntime_package ALL
                      COMMAND
                          mkdir -p ${ONNXRUNTIME_PACKAGE_DIR}/bin &&
                          mkdir -p ${ONNXRUNTIME_PACKAGE_DIR}/lib &&
                          cp ${ONNXRUNTIME_SOURCE_DIR}/build/Linux/${CMAKE_BUILD_TYPE}/libonnxruntime_providers_openvino.so ${ONNXRUNTIME_PACKAGE_DIR}/lib/ &&
                          cp ${ONNXRUNTIME_SOURCE_DIR}/build/Linux/${CMAKE_BUILD_TYPE}/libonnxruntime_providers_shared.so ${ONNXRUNTIME_PACKAGE_DIR}/lib/ &&
                          cp ${ONNXRUNTIME_SOURCE_DIR}/build/Linux/${CMAKE_BUILD_TYPE}/onnxruntime_perf_test ${ONNXRUNTIME_PACKAGE_DIR}/bin/ &&
                          tar -C ${ONNXRUNTIME_PACKAGE_DIR} -czf ${CMAKE_BINARY_DIR}/${ONNXRUNTIME_PACKAGE_NAME}.tar.gz .
                      DEPENDS onnxruntime_build
                      BYPRODUCTS ${CMAKE_BINARY_DIR}/${ONNXRUNTIME_PACKAGE_NAME}.tar.gz)
    install(
      FILES ${CMAKE_BINARY_DIR}/${ONNXRUNTIME_PACKAGE_NAME}.tar.gz
      DESTINATION .
      COMPONENT openvino_standalone_package
      EXCLUDE_FROM_ALL)
endif()

### OpenCV ###
set(OPENCV_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/src/opencv")
file(MAKE_DIRECTORY ${OPENCV_SOURCE_DIR})

set(OPENCV_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/build-opencv")
file(MAKE_DIRECTORY ${OPENCV_BINARY_DIR})
set(OPENCV_PACKAGE_DIR "${OPENVINO_PACKAGE_DIR}/opencv")

ExternalProject_Add(
  opencv_build
  GIT_REPOSITORY https://github.com/opencv/opencv.git
  GIT_TAG ${OPENCV_REVISION}
  DEPENDS openvino_build
  UPDATE_DISCONNECTED TRUE
  PATCH_COMMAND ""
  SOURCE_DIR ${OPENCV_SOURCE_DIR}
  BINARY_DIR ${OPENCV_BINARY_DIR}
  INSTALL_DIR ${OPENCV_PACKAGE_DIR}
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DCMAKE_INSTALL_PREFIX=${OPENCV_PACKAGE_DIR}
    -DCMAKE_PREFIX_PATH=${OPENVINO_BINARY_DIR}
    -DOPENCV_GENERATE_SETUPVARS=ON
    -DBUILD_opencv_python3=OFF
    -DBUILD_opencv_dnn=OFF
    -DWITH_EIGEN=OFF
    -DWITH_JASPER=OFF
    -DWITH_OPENCL=OFF
    -DWITH_OPENEXR=OFF
    -DWITH_OPENJPEG=OFF
    -DWITH_OPENVINO=ON
    -DWITH_TIFF=OFF
    -DWITH_WEBP=OFF)

# manually add an interface library for OpenCV so umd tests can link with it
add_library(opencv_core INTERFACE)
add_dependencies(opencv_core opencv_build)
target_include_directories(opencv_core INTERFACE ${OPENCV_PACKAGE_DIR}/include/opencv4)
target_link_libraries(opencv_core INTERFACE ${OPENCV_PACKAGE_DIR}/lib/libopencv_core.so)

add_library(opencv_imgcodecs INTERFACE)
add_dependencies(opencv_imgcodecs opencv_build)
target_include_directories(opencv_imgcodecs INTERFACE ${OPENCV_PACKAGE_DIR}/include/opencv4)
target_link_libraries(opencv_imgcodecs INTERFACE ${OPENCV_PACKAGE_DIR}/lib/libopencv_imgcodecs.so)

### single-image-test ###
ExternalProject_Add(
  single_image_test_build
  DOWNLOAD_COMMAND ""
  DEPENDS opencv_build
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}
  BINARY_DIR ${OPENVINO_BINARY_DIR}
  INSTALL_DIR ${OPENVINO_PACKAGE_DIR}
  CMAKE_ARGS
    -DOpenCV_DIR=${OPENCV_BINARY_DIR}
  BUILD_COMMAND
    ${CMAKE_COMMAND}
    --build ${OPENVINO_BINARY_DIR}
    --target single-image-test)

### Sample applications from OpenVINO (benchmark_app ...) ###
set(SAMPLES_APPS_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build-samples)
file(MAKE_DIRECTORY ${SAMPLES_APPS_BUILD_DIR})

set(SAMPLES_APPS_PACKAGE_DIR "${OPENVINO_PACKAGE_DIR}/tools/")
file(MAKE_DIRECTORY ${SAMPLES_APPS_PACKAGE_DIR})

ExternalProject_Add(
  sample_apps_build
  DOWNLOAD_COMMAND ""
  DEPENDS single_image_test_build
  PREFIX ${OPENVINO_PREFIX_DIR}
  SOURCE_DIR ${OPENVINO_SOURCE_DIR}/samples/cpp
  BINARY_DIR ${SAMPLES_APPS_BUILD_DIR}
  INSTALL_DIR ""
  CMAKE_ARGS
    ${COMMON_CMAKE_ARGS}
    -DCMAKE_PREFIX_PATH=${OPENVINO_BINARY_DIR}
    -DOpenCV_DIR=${OPENCV_BINARY_DIR}
    -DSAMPLES_ENABLE_OPENCL=OFF)

### OpenVINO package ###
set(OPENVINO_BINARY_RELEASE_DIR "${OPENVINO_SOURCE_DIR}/bin/intel64/Release")
set(OPENVINO_LIBRARY_DIR ${OPENVINO_BINARY_RELEASE_DIR} PARENT_SCOPE)
set(OPENCV_LIBRARY_DIR "${OPENCV_BINARY_DIR}/lib" PARENT_SCOPE)

add_custom_target(
  copy_to_openvino_package ALL
  COMMAND
    cp -d ${OPENCV_BINARY_DIR}/setup_vars.sh ${OPENCV_PACKAGE_DIR}/setupvars.sh &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/benchmark_app ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/classification_sample_async ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/hello_classification ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    cp -d ${SAMPLES_APPS_BUILD_DIR}/intel64/hello_query_device ${SAMPLES_APPS_PACKAGE_DIR}/ &&
    git -C ${OPENCV_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/opencv_sha &&
    git -C ${OPENVINO_SOURCE_DIR} rev-list --max-count=1 HEAD > ${OPENVINO_PACKAGE_DIR}/openvino_sha &&
    echo ${OPENVINO_PACKAGE_NAME} > ${OPENVINO_PACKAGE_DIR}/build_version &&
    echo `git -C ${OPENVINO_SOURCE_DIR} rev-parse HEAD` `git -C ${OPENVINO_SOURCE_DIR} config --local --get remote.origin.url` > ${OPENVINO_PACKAGE_DIR}/manifest.txt
  DEPENDS openvino_build opencv_build sample_apps_build single_image_test_build)

set(OPENVINO_PACKAGE_DEPS copy_to_openvino_package)

if (${BUILD_GENAI})
  set(WHEELS_PACKAGE_DIR "${OPENVINO_PACKAGE_DIR}/wheels")
  file(MAKE_DIRECTORY ${WHEELS_PACKAGE_DIR})
  set(GENAI_TOOLS_PACKAGE_DIR ${OPENVINO_PACKAGE_DIR}/genai_tools)
  file(MAKE_DIRECTORY ${GENAI_TOOLS_PACKAGE_DIR})
  add_custom_target(
    copy_wheels_to_openvino_package ALL
    COMMAND
      cp -d ${WHEELS_DIR}/openvino-*_x86_64.whl ${WHEELS_PACKAGE_DIR}/ &&
      cp -d ${WHEELS_DIR}/openvino_genai-*_x86_64.whl ${WHEELS_PACKAGE_DIR}/ &&
      cp -d ${WHEELS_DIR}/openvino_tokenizers-*-py3-none-linux_x86_64.whl ${WHEELS_PACKAGE_DIR}/ &&
      cp -dr ${GENAI_SOURCE_DIR}/tools/llm_bench ${GENAI_TOOLS_PACKAGE_DIR}/
    DEPENDS
      genai_build)
  list(APPEND OPENVINO_PACKAGE_DEPS copy_wheels_to_openvino_package)
endif()

add_custom_target(
  openvino_package ALL
  COMMAND
    tar -C ${OPENVINO_PACKAGE_DIR} -czf ${CMAKE_BINARY_DIR}/${OPENVINO_PACKAGE_NAME}.tar.gz .
  DEPENDS ${OPENVINO_PACKAGE_DEPS}
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

if (${BUILD_GENAI})
  install(DIRECTORY ${WHEELS_DIR}/
          COMPONENT openvino-npu
          DESTINATION ${VALIDATION_INSTALL_DATADIR}/wheels
          FILES_MATCHING
          PATTERN "openvino-*_x86_64.whl"
          PATTERN "openvino_genai-*_x86_64.whl"
          PATTERN "openvino_tokenizers-*-py3-none-linux_x86_64.whl")
  install(DIRECTORY ${GENAI_SOURCE_DIR}/tools/llm_bench
          COMPONENT openvino-npu
          DESTINATION ${VALIDATION_INSTALL_DATADIR}/genai_tools)
endif()
