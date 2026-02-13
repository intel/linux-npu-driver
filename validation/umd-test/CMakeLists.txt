#
# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

project(npu-umd-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(utilities)

find_package(OpenVINO QUIET COMPONENTS Runtime)
find_package(OpenCV QUIET)
find_package(OpenCL QUIET)

add_compile_options(-Wno-error=ignored-qualifiers)

add_executable(${PROJECT_NAME}
    graph_utilities.cpp
    image.cpp
    main.cpp
    umd_test.cpp
    test_commands.cpp
    test_context.cpp
    test_copy_performance.cpp
    test_device.cpp
    test_driver.cpp
    test_driver_cache.cpp
    test_event.cpp
    test_event_sync.cpp
    test_eventpool.cpp
    test_external_memory.cpp
    test_external_memory_dma_heap.cpp
    test_external_memory_ze.cpp
    test_fence.cpp
    test_graph.cpp
    test_graph_cid.cpp
    test_graph_commands.cpp
    test_graph_inference.cpp
    test_graph_query.cpp
    test_immediate_cmdlist.cpp
    test_idle_optimizations.cpp
    test_import_system_memory.cpp
    test_inference_performance.cpp
    test_init_drivers.cpp
    test_inorder_exec.cpp
    test_memory.cpp
    test_metric.cpp
    test_metric_streamer.cpp
    test_mutable_cmdlist.cpp
    test_tensor_strides.cpp
    test_priority.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/linux/include/uapi")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/umd/level_zero_driver/api/prv/")

if (TARGET openvino::runtime)
    target_sources(${PROJECT_NAME} PRIVATE test_ov_inference.cpp)
    target_link_libraries(${PROJECT_NAME} openvino::runtime)
    # TODO: Temporarily disable L0 validation for OpenVINO till Driver CI switch to OpenVINO that
    # includes Graph Extension "stype" adjustments.:
    # https://github.com/openvinotoolkit/openvino/commit/e496aed4d243702a8087c4981de0b15aaf34e9b9
    target_compile_definitions(${PROJECT_NAME} PUBLIC L0_VALIDATION_DISABLE)
endif()

if (TARGET opencv_core AND TARGET opencv_imgcodecs)
    target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgcodecs)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UMD_TESTS_USE_OPENCV)
endif()

if (TARGET OpenCL::OpenCL)
    target_sources(${PROJECT_NAME} PRIVATE test_external_memory_opencl.cpp)
    target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -DVPU_GTEST_APP -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME} ze_loader test_app_lib yaml-cpp data_handle fw_vpu_api_headers)
install(TARGETS ${PROJECT_NAME}
        COMPONENT validation-npu)
