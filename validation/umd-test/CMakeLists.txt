#
# Copyright (C) 2022-2024 Intel Corporation
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

add_executable(${PROJECT_NAME}
    blob_params.cpp
    image.cpp
    model_params.cpp
    main.cpp
    umd_test.cpp
    test_buffers_export.cpp
    test_buffers_import.cpp
    test_buffers_export_gpu.cpp
    test_commands.cpp
    test_context.cpp
    test_copy_performance.cpp
    test_device.cpp
    test_driver.cpp
    test_event.cpp
    test_event_sync.cpp
    test_eventpool.cpp
    test_fence.cpp
    test_graph.cpp
    test_graph_cid.cpp
    test_graph_commands.cpp
    test_graph_inference.cpp
    test_graph_query.cpp
    test_immediate_cmdlist.cpp
    test_inference_performance.cpp
    test_memory.cpp
    test_metric.cpp
    test_metric_streamer.cpp
    test_priority.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/linux/include/uapi")

if (OpenVINO_FOUND)
    target_sources(${PROJECT_NAME} PRIVATE test_ov_inference.cpp test_mutable_cmdlist.cpp)
    target_link_libraries(${PROJECT_NAME} openvino::runtime)
    target_compile_definitions(${PROJECT_NAME} PUBLIC UMD_TESTS_USE_OPENVINO=1)
endif()

if (OpenCV_FOUND)
    target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgcodecs)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UMD_TESTS_USE_OPENCV)
endif()

if (OpenCL_FOUND)
    target_sources(${PROJECT_NAME} PRIVATE test_buffers_import_gpu.cpp)
    target_link_libraries(${PROJECT_NAME} OpenCL)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -DVPU_GTEST_APP -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME} ze_loader test_app_lib yaml-cpp data_handle)
install(TARGETS ${PROJECT_NAME}
        COMPONENT validation-npu)
